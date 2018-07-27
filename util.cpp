#include "util.h"

//#include <qlocalsocket.h>
bool TestConnection(QString ip)
{
    QTcpSocket socket(new QTcpSocket());
    socket.connectToHost(ip, 5555);
    if (socket.waitForConnected(3000))
    {
        return true;
    }
    else
    {
        qDebug() << socket.errorString();
        return false;
    }
}
