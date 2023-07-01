#pragma once

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class QLabel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

  public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

  protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

  private slots:
    void slot_calc();

  private:
    Ui::MainWindow *ui;
    bool replace;
};
