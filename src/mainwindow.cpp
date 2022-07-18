#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QPainter>
#include <QDebug>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_ccr(new CCR(this))
{
    ui->setupUi(this);
    setWindowTitle(tr("助航灯调试上位机V1.0"));
    setWindowIcon(QIcon(":/images/app.ico"));
    this->resize(900, 600);
    initToolButton();
    connect(ui->tBtnCCR, &QToolButton::clicked, [=](){
        this->hide();
        m_ccr->show();
    });

    connect(m_ccr, &CCR::backToHomepage, [=](){
        m_ccr->hide();
        this->show();
    });
}

MainWindow::~MainWindow()
{
    delete ui;
    qDebug()<<"mainwindow destroyed1";
}

/**
 * @brief MainWindow::initToolButton
 * 初始化主界面工具按钮控件
 */
void MainWindow::initToolButton()
{

    QVector< QToolButton *> list;
    list<<ui->tBtnCCR<<ui->tBtnMessure<<ui->tBtnSwitchChest
         <<ui->tBtnBlinkLight<<ui->tBtnReserved;
    QVector< QToolButton *>::iterator i;
    const char * tBtnNames[5] = {QT_TR_NOOP("调光器"),
                                 QT_TR_NOOP("坏灯数/绝缘电阻"),
                                 QT_TR_NOOP("高压切换柜"),
                                 QT_TR_NOOP("闪光灯"),
                                 QT_TR_NOOP("预留"),
                                };
    int j = 0;
    for(i=list.begin();  i!=list.end(); i++, j++)
    {
        QFont font("幼圆",14,2);
        font.setBold(true);
        (*i)->setToolButtonStyle(Qt::ToolButtonStyle::ToolButtonTextUnderIcon);
        (*i)->setFont(font);
        (*i)->setFixedSize(130, 130);
        (*i)->setIcon(QIcon(QString(":/images/tbtn%1.png").arg(j+1)));
        (*i)->setIconSize(QSize(100, 100));
        (*i)->setText(tr(tBtnNames[j]));
        (*i)->setAutoRaise(true);
    }
}

void MainWindow::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QPixmap pixmap(":/images/background.png");
    painter.drawPixmap(rect(), pixmap, QRect());
    QPixmap pixmapTitle(":/images/title.png");
    painter.drawPixmap(QRect(QPoint(0, 0), QPoint(900, 100)), pixmapTitle, QRect());
}
