#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    MainWindow *ww;


private slots:
    void on_Caesar_clicked();

    void on_Keyword_clicked();

    void on_Vigenere_clicked();

    void on_Autokey_clicked();

    void on_Playfair_clicked();

    void on_Permutation_clicked();

    void on_C_Permutation_clicked();

    void on_RC4_clicked();

    void on_DES_clicked();

    void on_RSA_clicked();

    void slot_1();
    void slot_2();
    void slot_3();
    void slot_4();
    void slot_5();
    void slot_6();
    void slot_7();
    void slot_8();
    void slot_9();
    void slot_10();
    void slot_11();

    void on_file_clicked();

    void on_Encrypt_clicked();

    void on_Decrypt_clicked();

    void on_Clean_clicked();

    void on_Socket_clicked();

    void on_connect_clicked();

    void on_Send_clicked();

    void on_Receive_clicked();

    void on_disconnect_clicked();

    void on_DH_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
