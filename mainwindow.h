#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <Qt>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    char* getSearchSize(void);
    char* getFreezeSize(void);
    char* getPokeSize(void);
    void connectErr(void);
    void resetfreezeids(void);

private slots:
    void on_connect_released();

    void on_searchbutton_released();

    void on_newsearch_clicked();

    void on_freeze_button_clicked();

    void on_unfreezebutton_clicked();

    void on_pokebutton_clicked();

private:
    Ui::MainWindow *ui;
    bool isNewSearch;
};

#endif // MAINWINDOW_H
