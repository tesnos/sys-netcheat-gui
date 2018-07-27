#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "util.h"

#include <QMessageBox>
#include <QtNetwork/QLocalSocket>
#include <QProgressDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    bool isNewSearch = true;
    ui->searchtree->setColumnCount(1);
    ui->searchtree->setHeaderLabels(QStringList() << "Location");
    ui->freezetree->setColumnCount(3);
    ui->freezetree->setHeaderLabels(QStringList() << "ID" << "Location" << "Val");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::connectErr()
{
    QMessageBox msgBox;
    msgBox.setText("The switch could not be found, please double check everything.");
    msgBox.exec();
}

char* MainWindow::getSearchSize()
{
    if (ui->u8_search->isChecked())
    {
        return "u8\0";
    }
    else if (ui->u16_search->isChecked())
    {
        return "u16\0";
    }
    else if (ui->u32_search->isChecked())
    {
        return "u32\0";
    }
    else if (ui->u64_search->isChecked())
    {
        return "u64\0";
    }
    else
    {
        return "u32\0";
        QMessageBox msgBox;
        msgBox.setText("Please select a search size before searching, default u32 was used");
        msgBox.exec();
    }
}

char* MainWindow::getFreezeSize()
{
    if (ui->u8_freeze->isChecked())
    {
        return "u8\0";
    }
    else if (ui->u16_freeze->isChecked())
    {
        return "u16\0";
    }
    else if (ui->u32_freeze->isChecked())
    {
        return "u32\0";
    }
    else if (ui->u64_freeze->isChecked())
    {
        return "u64\0";
    }
    else
    {
        return "u32\0";
        QMessageBox msgBox;
        msgBox.setText("Please select a freeze size before freezing, default u32 was used");
        msgBox.exec();
    }
}

char* MainWindow::getPokeSize()
{
    if (ui->u8_poke->isChecked())
    {
        return "u8\0";
    }
    else if (ui->u16_poke->isChecked())
    {
        return "u16\0";
    }
    else if (ui->u32_poke->isChecked())
    {
        return "u32\0";
    }
    else if (ui->u64_poke->isChecked())
    {
        return "u64\0";
    }
    else
    {
        return "u32\0";
        QMessageBox msgBox;
        msgBox.setText("Please select a poke size before poking, default u32 was used");
        msgBox.exec();
    }
}

void MainWindow::on_connect_released()
{
    if(TestConnection(ui->ip_box->text()))
    {
        QMessageBox msgBox;
        msgBox.setText("Switch connected!");
        msgBox.exec();
    }
    else
    {
        connectErr();
    }
}

void MainWindow::on_searchbutton_released()
{
    QTcpSocket socket(new QTcpSocket());
    socket.connectToHost(ui->ip_box->text(), 5555);
    if (socket.waitForConnected(3000))
    {
        QString searchreq = nullptr;
        if (isNewSearch)
        {
            searchreq = QString("ssearch %1 %2\n\0").arg(getSearchSize()).arg(ui->search->text());
            isNewSearch = false;
        }
        else
        {
            searchreq = QString("csearch %1\n\0").arg(ui->search->text());
        }

        //socket.write("hey hey hey");
        QByteArray ba = searchreq.toLatin1();
        //qDebug() << ba.data();
        char introbuf[256];
        socket.waitForReadyRead();
        socket.read(introbuf, 256);
        //qDebug() << introbuf;
        socket.waitForReadyRead(500);
        socket.read(introbuf, 256);
        //qDebug() << introbuf;
        socket.write(ba.data());
        socket.waitForBytesWritten();

        ui->searchtree->clear();

        //char linebuf[50];
        socket.waitForReadyRead(250);
        long long int bytesread = 0;
        QList<QTreeWidgetItem *> treeitems = QList<QTreeWidgetItem*>();
        while(true)
        {
            //memset(linebuf, 0, 50);
            QByteArray bytes = socket.readAll();
            if (bytesread == 0) { break; }
            //qDebug() << bytes.data();
            bool last = false;
            if (bytes.data()[bytes.size() - 2] == 0x3e) { last = true; }
            if (bytes.contains(0x21))
            {
                for (int i = 0; i < bytes.size(); i++)
                {
                    QByteArray test = bytes;
                    int location_len = 0;
                    if (test.data()[i] == 0x47)
                    {
                        for (int j = i; j < test.size(); j++)
                        {
                            if (test.data()[j] == 0x21)
                            {
                                location_len = j - (i + 13);
                                break;
                            }
                        }
                    } else { continue; }
                    test.remove(0, i);
                    test.remove(i + 14 + location_len, test.size() - (i + 14 + location_len));
                    test.remove(0, 13);
                    test.truncate(location_len);
                    //if(test.data()[test.size()] == 0x30) { test.append(0x30); }
                    //qDebug() << i + 1 << "/" << bytes.size() / 24 << ":" << test;
                    QTreeWidgetItem* treeitem = new QTreeWidgetItem(ui->searchtree);
                    QString qlinebuf = QString(test);
                    //qDebug() << "raw:\n" << qlinebuf;
                    //qlinebuf.remove(22, qlinebuf.size() - 23);
                    //qlinebuf.remove(0, 13);
                    //qDebug() << "formatted:\n" << qlinebuf;
                    treeitem->setText(0, qlinebuf);
                    treeitems.append(treeitem);
                }
            }
            if (last) { break; }// else { qDebug() << "belugh" << bytes.data()[bytes.size() - 2]; }

            if (!socket.waitForReadyRead(5000)) { break; }

        }
        ui->searchtree->addTopLevelItems(treeitems);
        ui->searchtree->update();

        socket.disconnect();
    }
    else
    {
        MainWindow::connectErr();
    }
}
//192.168.1.142

void MainWindow::on_newsearch_clicked()
{
    isNewSearch = true;
}

void MainWindow::on_freeze_button_clicked()
{
    QTcpSocket socket(new QTcpSocket());
    socket.connectToHost(ui->ip_box->text(), 5555);
    if (socket.waitForConnected(3000))
    {
        QString freezereq = nullptr;
        freezereq = QString("afreeze %1 %2 %3\n\0").arg(ui->freezeloc->text()).arg(getFreezeSize()).arg(ui->freezeval->text());

        QByteArray ba = freezereq.toLatin1();
        char introbuf[256];
        socket.waitForReadyRead();
        socket.read(introbuf, 256);
        //qDebug() << introbuf;
        socket.waitForReadyRead(50);
        socket.read(introbuf, 256);
        //qDebug() << introbuf;
        //qDebug() << ba.data();
        socket.write(ba.data());
        socket.waitForBytesWritten();


        QString qidbuf = QString("%1").arg(ui->freezetree->topLevelItemCount());
        QTreeWidgetItem* treeitem = new QTreeWidgetItem(ui->freezetree);
        treeitem->setText(0, qidbuf);
        treeitem->setText(1, ui->freezeloc->text());
        treeitem->setText(2, ui->freezeval->text());
        ui->freezetree->addTopLevelItem(treeitem);
        ui->freezetree->update();

        socket.disconnect();
    }
    else
    {
        MainWindow::connectErr();
    }
}

void MainWindow::resetfreezeids()
{
    QTreeWidgetItem* item;
    int itemrow;
    for (itemrow = 0; itemrow < ui->freezetree->topLevelItemCount(); itemrow++)
    {
        item = ui->freezetree->itemAt(0, itemrow);
        QString newid = QString("%1").arg(ui->unfreeze_selector->text());
        item->setText(0, newid);
    }
}

void MainWindow::on_unfreezebutton_clicked()
{
    bool founditem = false;
    QTreeWidgetItem* item;
    int itemrow;
    for (itemrow = 0; itemrow < ui->freezetree->topLevelItemCount(); itemrow++)
    {
        item = ui->freezetree->itemAt(0, itemrow);
        QVariant itemID = item->data(0, Qt::DisplayRole);
        QString itemIDstr = itemID.toString();
        if (itemIDstr.compare(ui->unfreeze_selector->text()) == 0)
        {
            founditem = true;
            break;
        }
    }
    if (founditem == false)
    {
        return;
    }

    QTcpSocket socket(new QTcpSocket());
    socket.connectToHost(ui->ip_box->text(), 5555);
    if (socket.waitForConnected(3000))
    {
        QString freezereq = nullptr;
        freezereq = QString("dfreeze %1\n\0").arg(ui->unfreeze_selector->text());

        QByteArray ba = freezereq.toLatin1();
        char introbuf[256];
        socket.waitForReadyRead();
        socket.read(introbuf, 256);
        //qDebug() << introbuf;
        socket.waitForReadyRead(50);
        socket.read(introbuf, 256);
        //qDebug() << introbuf;
        //qDebug() << ba.data();
        socket.write(ba.data());
        socket.waitForBytesWritten();

        ui->freezetree->takeTopLevelItem(itemrow);
        ui->freezetree->update();
        resetfreezeids();

        socket.disconnect();
    }
    else
    {
        MainWindow::connectErr();
    }
}

void MainWindow::on_pokebutton_clicked()
{
    QTcpSocket socket(new QTcpSocket());
    socket.connectToHost(ui->ip_box->text(), 5555);
    if (socket.waitForConnected(3000))
    {
        QString freezereq = nullptr;
        freezereq = QString("poke %1 %2 %3\n\0").arg(ui->pokeaddr_box->text()).arg(getPokeSize()).arg(ui->pokeval_box->text());

        QByteArray ba = freezereq.toLatin1();
        char introbuf[256];
        socket.waitForReadyRead();
        socket.read(introbuf, 256);
        //qDebug() << introbuf;
        socket.waitForReadyRead(50);
        socket.read(introbuf, 256);
        //qDebug() << introbuf;
        //qDebug() << ba.data();
        socket.write(ba.data());
        socket.waitForBytesWritten();
        socket.disconnect();
    }
    else
    {
        MainWindow::connectErr();
    }
}
