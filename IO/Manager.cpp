/*
 * Copyright (c) 2020-2022 Alex Spataru <https://github.com/alex-spataru>
 * Copyright (c) 2021 JP Norair <https://github.com/jpnorair>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <IO/Manager.h>
#include <IO/Checksum.h>
#include <IO/Drivers/Serial.h>
#include <IO/Drivers/Network.h>
#include <IO/Drivers/BluetoothLE.h>

#include <MQTT/Client.h>

/**
 * Adds support for C escape sequences to the given @a str.
 * When user inputs "\n" in a textbox, Qt automatically converts that string to "\\n".
 * For our purposes, we need to convert "\\n" back to "\n", and so on with the rest of
 * the escape sequences supported by C.
 *
 * TODO: add support for numbers
 */
static QString ADD_ESCAPE_SEQUENCES(const QString &str)
{
    auto escapedStr = str;
    escapedStr = escapedStr.replace("\\a", "\a");
    escapedStr = escapedStr.replace("\\b", "\b");
    escapedStr = escapedStr.replace("\\f", "\f");
    escapedStr = escapedStr.replace("\\n", "\n");
    escapedStr = escapedStr.replace("\\r", "\r");
    escapedStr = escapedStr.replace("\\t", "\t");
    escapedStr = escapedStr.replace("\\v", "\v");
    return escapedStr;
}

/**
 * Constructor function
 */
IO::Manager::Manager()
    : m_enableCrc(false)
    , m_writeEnabled(true)
    , m_maxBufferSize(1024 * 1024)
    , m_driver(Q_NULLPTR)
    , m_receivedBytes(0)
    , m_startSequence("/*")
    , m_finishSequence("*/")
    , m_separatorSequence(",")
{
    // Set initial settings
    setMaxBufferSize(1024 * 1024);
    setSelectedDriver(SelectedDriver::Serial);

    // clang-format off

    // Update connect button status when device type is changed
    connect(this, &IO::Manager::selectedDriverChanged,
            this, &IO::Manager::configurationChanged);

    // clang-format on
}

/**
 * Returns the only instance of the class
 */
IO::Manager &IO::Manager::instance()
{
    static Manager singleton;
    return singleton;
}

/**
 * Returns @c true if a device is connected and its open in read-only mode
 */
bool IO::Manager::readOnly()
{
    return connected() && !m_writeEnabled;
}

/**
 * Returns @c true if a device is connected and its open in read/write mode
 */
bool IO::Manager::readWrite()
{
    return connected() && m_writeEnabled;
}

/**
 * Returns @c true if a device is currently selected and opened or if the MQTT client
 * is currently connected as a subscriber.
 */
bool IO::Manager::connected()
{
    if (driver())
        return driver()->isOpen();

    return MQTT::Client::instance().isSubscribed();
}

/**
 * Returns @c true if a device is currently selected
 */
bool IO::Manager::deviceAvailable()
{
    return driver() != Q_NULLPTR;
}

/**
 * Returns @c true if we are able to connect to a device/port with the current config.
 */
bool IO::Manager::configurationOk()
{
    if (driver())
        return driver()->configurationOk();

    return false;
}

/**
 * Returns the maximum size of the buffer. This is useful to avoid consuming to much
 * memory when a large block of invalid data is received (for example, when the user
 * selects an invalid baud rate configuration).
 */
int IO::Manager::maxBufferSize() const
{
    return m_maxBufferSize;
}

/**
 * Returns a pointer to the currently selected driver.
 *
 * @warning you need to check this pointer before using it, it can have a @c Q_NULLPTR
 *          value during normal operations.
 */
IO::HAL_Driver *IO::Manager::driver()
{
    return m_driver;
}

/**
 * Returns the currently selected data source, possible return values:
 * - @c DataSource::Serial  use a serial port as a data source
 * - @c DataSource::Network use a network port as a data source
 */
IO::Manager::SelectedDriver IO::Manager::selectedDriver() const
{
    return m_selectedDriver;
}

/**
 * Returns the start sequence string used by the application to know where to consider
 * that a frame begins. If the start sequence is empty, then the application shall ignore
 * incoming data. The only thing that wont ignore the incoming data will be the console.
 */
QString IO::Manager::startSequence() const
{
    return m_startSequence;
}

/**
 * Returns the finish sequence string used by the application to know where to consider
 * that a frame ends. If the start sequence is empty, then the application shall ignore
 * incoming data. The only thing that wont ignore the incoming data will be the console.
 */
QString IO::Manager::finishSequence() const
{
    return m_finishSequence;
}

/**
 * Returns the separator sequence string used by the application to know where to consider
 * that a data item ends.
 */
QString IO::Manager::separatorSequence() const
{
    return m_separatorSequence;
}

/**
 * Returns a list with the possible data source options.
 */
StringList IO::Manager::availableDrivers() const
{
    StringList list;
    list.append(tr("Serial port"));
    list.append(tr("Network port"));
    list.append(tr("Bluetooth LE device"));
    return list;
}

/**
 * Tries to write the given @a data to the current device.
 *
 * @returns the number of bytes written to the target device
 */
qint64 IO::Manager::writeData(const QByteArray &data)
{
    if (connected())
    {
        // Write data to device
        auto bytes = driver()->write(data);

        // Show sent data in console
        if (bytes > 0)
        {
            auto writtenData = data;
            writtenData.chop(data.length() - bytes);
            Q_EMIT dataSent(writtenData);
        }

        // Return number of bytes written
        return bytes;
    }

    return -1;
}

/**
 * Connects/disconnects the application from the currently selected device. This function
 * is used as a convenience for the connect/disconnect button.
 */
void IO::Manager::toggleConnection()
{
    if (connected())
        disconnectDriver();
    else
        connectDevice();
}

/**
 * Closes the currently selected device and tries to open & configure a new connection.
 * A data source must be selected before calling this function.
 */
void IO::Manager::connectDevice()
{
    // Reset driver
    setSelectedDriver(selectedDriver());

    // Configure current device
    if (deviceAvailable())
    {
        // Set open flag
        QIODevice::OpenMode mode = QIODevice::ReadOnly;
        if (m_writeEnabled)
            mode = QIODevice::ReadWrite;

        // Open device
        if (driver()->open(mode))
        {
            connect(driver(), &IO::HAL_Driver::dataReceived, this,
                    &IO::Manager::onDataReceived);
        }

        // Error opening the device
        else
            disconnectDriver();

        // Update UI
        Q_EMIT connectedChanged();
    }
}

/**
 * Disconnects from the current device and clears temp. data
 */
void IO::Manager::disconnectDriver()
{
    if (deviceAvailable())
    {
        // Disconnect device signals/slots
        disconnect(driver(), &IO::HAL_Driver::dataReceived, this,
                   &IO::Manager::onDataReceived);
        disconnect(driver(), &IO::HAL_Driver::configurationChanged, this,
                   &IO::Manager::configurationChanged);

        // Close driver device
        driver()->close();

        // Update device pointer
        m_driver = Q_NULLPTR;
        m_receivedBytes = 0;
        m_dataBuffer.clear();
        m_dataBuffer.reserve(maxBufferSize());

        // Update UI
        Q_EMIT driverChanged();
        Q_EMIT connectedChanged();
    }

    // Disable CRC checking
    m_enableCrc = false;
}

/**
 * Enables/disables RW mode
 */
void IO::Manager::setWriteEnabled(const bool enabled)
{
    m_writeEnabled = enabled;
    Q_EMIT writeEnabledChanged();
}

/**
 * Reads the given payload and emits it as if it were received from a device.
 * This function is for convenience to interact with other application modules & plugins.
 */
void IO::Manager::processPayload(const QByteArray &payload)
{
    if (!payload.isEmpty())
    {
        // Update received bytes indicator
        m_receivedBytes += payload.size();
        if (m_receivedBytes >= UINT64_MAX)
            m_receivedBytes = 0;

        // Notify user interface & application modules
        Q_EMIT dataReceived(payload);
        Q_EMIT frameReceived(payload);
        Q_EMIT receivedBytesChanged();
    }
}

/**
 * Changes the maximum permited buffer size. Check the @c maxBufferSize() function for
 * more information.
 */
void IO::Manager::setMaxBufferSize(const int maxBufferSize)
{
    m_maxBufferSize = maxBufferSize;
    Q_EMIT maxBufferSizeChanged();

    m_dataBuffer.reserve(maxBufferSize);
}

/**
 * Changes the frame start sequence. Check the @c startSequence() function for more
 * information.
 */
void IO::Manager::setStartSequence(const QString &sequence)
{
    m_startSequence = ADD_ESCAPE_SEQUENCES(sequence);
    if (m_startSequence.isEmpty())
        m_startSequence = "/*";

    Q_EMIT startSequenceChanged();
}

/**
 * Changes the frame end sequence. Check the @c finishSequence() function for more
 * information.
 */
void IO::Manager::setFinishSequence(const QString &sequence)
{
    m_finishSequence = ADD_ESCAPE_SEQUENCES(sequence);
    if (m_finishSequence.isEmpty())
        m_finishSequence = "*/";

    Q_EMIT finishSequenceChanged();
}

/**
 * Changes the frame separator sequence. Check the @c separatorSequence() function for
 * more information.
 */
void IO::Manager::setSeparatorSequence(const QString &sequence)
{
    m_separatorSequence = ADD_ESCAPE_SEQUENCES(sequence);
    if (m_separatorSequence.isEmpty())
        m_separatorSequence = ",";

    Q_EMIT separatorSequenceChanged();
}

/**
 * Changes the data source type. Check the @c dataSource() funciton for more information.
 */
void IO::Manager::setSelectedDriver(const IO::Manager::SelectedDriver &driver)
{
    // Disconnect current driver
    disconnectDriver();

    // Change data source
    m_selectedDriver = driver;

    // Disconnect previous device (if any)
    disconnectDriver();

    // Try to open a serial port connection
    if (selectedDriver() == SelectedDriver::Serial)
        setDriver(&(Drivers::Serial::instance()));

    // Try to open a network connection
    else if (selectedDriver() == SelectedDriver::Network)
        setDriver(&(Drivers::Network::instance()));

    // Try to open a BLE connection
    else if (selectedDriver() == SelectedDriver::BluetoothLE)
        setDriver(&(Drivers::BluetoothLE::instance()));

    // Invalid driver
    else
        setDriver(Q_NULLPTR);

    // Update UI
    Q_EMIT selectedDriverChanged();
}

/**
 * Read frames from temporary buffer, every frame that contains the appropiate start/end
 * sequence is removed from the buffer as soon as its read.
 *
 * This function also checks that the buffer size does not exceed specified size
 * limitations.
 *
 * Implemementation credits: @jpnorair and @alex-spataru
 */
void IO::Manager::readFrames()
{
    // No device connected, abort
    if (!connected())
        return;

    // Read until start/finish combinations are not found
    auto bytes = 0;
    auto prevBytes = 0;
    auto cursor = m_dataBuffer;
    auto start = startSequence().toUtf8();
    auto finish = finishSequence().toUtf8();
    while (cursor.contains(start) && cursor.contains(finish))
    {
        // Remove the part of the buffer prior to, and including, the start sequence.
        auto sIndex = cursor.indexOf(start);
        cursor = cursor.mid(sIndex + start.length(), -1);
        bytes += sIndex + start.length();

        // Copy a sub-buffer that goes until the finish sequence
        auto fIndex = cursor.indexOf(finish);
        auto frame = cursor.left(fIndex);

        // Checksum verification & Q_EMIT RX frame
        int chop = 0;
        auto result = integrityChecks(frame, cursor, &chop);
        if (result == ValidationStatus::FrameOk)
            Q_EMIT frameReceived(frame);

        // Checksum data incomplete, try next time...
        else if (result == ValidationStatus::ChecksumIncomplete)
        {
            bytes = prevBytes;
            break;
        }

        // Remove the data including the finish sequence from the master buffer
        cursor = cursor.mid(fIndex + chop, -1);
        bytes += fIndex + chop;

        // Frame read successfully, save the number of bytes to chop.
        // This is used to manage frames with incomplete checksums
        prevBytes = bytes;
    }

    // Remove parsed data from master buffer
    m_dataBuffer.remove(0, bytes);

    // Clear temp. buffer (e.g. device sends a lot of invalid data)
    if (m_dataBuffer.size() > maxBufferSize())
        clearTempBuffer();
}

/**
 * Deletes the contents of the temporary buffer. This function is called automatically by
 * the class when the temporary buffer size exceeds the limit imposed by the
 * @c maxBufferSize() function.
 */
void IO::Manager::clearTempBuffer()
{
    m_dataBuffer.clear();
}

/**
 * Changes the target device pointer. Deletion should be handled by the interface
 * implementation, not by this class.
 */
void IO::Manager::setDriver(HAL_Driver *driver)
{
    if (driver)
        connect(driver, &IO::HAL_Driver::configurationChanged, this,
                &IO::Manager::configurationChanged);

    m_driver = driver;

    Q_EMIT driverChanged();
    Q_EMIT configurationChanged();
}

/**
 * Reads incoming data from the I/O device, updates the console object, registers incoming
 * data to temporary buffer & extracts valid data frames from the buffer using the @c
 * readFrame() function.
 */
void IO::Manager::onDataReceived(const QByteArray &data)
{
    // Verify that device is still valid
    if (!driver())
        disconnectDriver();

    // Read data & append it to buffer
    auto bytes = data.length();

    // Obtain frames from data buffer
    m_dataBuffer.append(data);
    readFrames();

    // Update received bytes indicator
    m_receivedBytes += bytes;
    if (m_receivedBytes >= UINT64_MAX)
        m_receivedBytes = 0;

    // Notify user interface
    Q_EMIT receivedBytesChanged();
    Q_EMIT dataReceived(data);
}

/**
 * Checks if the @c cursor has a checksum corresponding to the given @a frame.
 * If so, the function shall calculate the appropiate checksum to for the @a frame and
 * compare it with the received checksum to verify the integrity of received data.
 *
 * @param frame data in which we shall perform integrity checks
 * @param cursor master buffer, should start with checksum type header
 * @param bytes pointer to the number of bytes that we need to chop from the master buffer
 */
IO::Manager::ValidationStatus IO::Manager::integrityChecks(const QByteArray &frame,
                                                           const QByteArray &cursor,
                                                           int *bytes)
{
    // Get finish sequence as byte array
    auto finish = finishSequence().toUtf8();
    auto crc8Header = finish + "crc8:";
    auto crc16Header = finish + "crc16:";
    auto crc32Header = finish + "crc32:";

    // Check CRC-8
    if (cursor.contains(crc8Header))
    {
        // Enable the CRC flag
        m_enableCrc = true;
        auto offset = cursor.indexOf(crc8Header) + crc8Header.length() - 1;

        // Check if we have enough data in the buffer
        if (cursor.length() >= offset + 1)
        {
            // Increment the number of bytes to remove from master buffer
            *bytes += crc8Header.length() + 1;

            // Get 8-bit checksum
            const quint8 crc = cursor.at(offset + 1);

            // Compare checksums
            if (crc8(frame.data(), frame.length()) == crc)
                return ValidationStatus::FrameOk;
            else
                return ValidationStatus::ChecksumError;
        }
    }

    // Check CRC-16
    else if (cursor.contains(crc16Header))
    {
        // Enable the CRC flag
        m_enableCrc = true;
        auto offset = cursor.indexOf(crc16Header) + crc16Header.length() - 1;

        // Check if we have enough data in the buffer
        if (cursor.length() >= offset + 2)
        {
            // Increment the number of bytes to remove from master buffer
            *bytes += crc16Header.length() + 2;

            // Get 16-bit checksum
            const quint8 a = cursor.at(offset + 1);
            const quint8 b = cursor.at(offset + 2);
            const quint16 crc = (a << 8) | (b & 0xff);

            // Compare checksums
            if (crc16(frame.data(), frame.length()) == crc)
                return ValidationStatus::FrameOk;
            else
                return ValidationStatus::ChecksumError;
        }
    }

    // Check CRC-32
    else if (cursor.contains(crc32Header))
    {
        // Enable the CRC flag
        m_enableCrc = true;
        auto offset = cursor.indexOf(crc32Header) + crc32Header.length() - 1;

        // Check if we have enough data in the buffer
        if (cursor.length() >= offset + 4)
        {
            // Increment the number of bytes to remove from master buffer
            *bytes += crc32Header.length() + 4;

            // Get 32-bit checksum
            const quint8 a = cursor.at(offset + 1);
            const quint8 b = cursor.at(offset + 2);
            const quint8 c = cursor.at(offset + 3);
            const quint8 d = cursor.at(offset + 4);
            const quint32 crc = (a << 24) | (b << 16) | (c << 8) | (d & 0xff);

            // Compare checksums
            if (crc32(frame.data(), frame.length()) == crc)
                return ValidationStatus::FrameOk;
            else
                return ValidationStatus::ChecksumError;
        }
    }

    // Buffer does not contain CRC code
    else if (!m_enableCrc)
    {
        *bytes += finish.length();
        return ValidationStatus::FrameOk;
    }

    // Checksum data incomplete
    return ValidationStatus::ChecksumIncomplete;
}

#ifdef SERIAL_STUDIO_INCLUDE_MOC
#    include "moc_Manager.cpp"
#endif
