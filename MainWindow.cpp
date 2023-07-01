#include "MainWindow.h"
#include "./ui_MainWindow.h"

#include <QDebug>
#include <QFontDatabase>
#include <QKeyEvent>
#include <QButtonGroup>

MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent)
  , ui(new Ui::MainWindow)
  , replace(true)
{
  ui->setupUi(this);

  QButtonGroup *g = new QButtonGroup(this);
  g->addButton(ui->_20Radio);
  g->addButton(ui->_10Radio);
  g->addButton(ui->_55Radio);

  QFontDatabase db;
  QFont f;
  if ( !db.hasFamily("OCR B") ) {
    qInfo() << "OCR B font not found. Consider installing it (ubuntu package: fonts-ocr-b)";
    f = db.systemFont(QFontDatabase::FixedFont);
    f.setPointSize(28);
  } else {
    f = db.font("OCR B", QString(), 28);
  }

  auto w = QFontMetrics(f).horizontalAdvance("999999.99");

  ui->ttcEdit->setFont(f);
  ui->htEdit->setFont(f);
  ui->tvaEdit->setFont(f);

  ui->ttcEdit->setFixedWidth(w+20);
  ui->htEdit->setFixedWidth(w+20);
  ui->tvaEdit->setFixedWidth(w+20);

  ui->ttcEdit->installEventFilter(this);
  ui->htEdit->installEventFilter(this);
  //ui->tvaEdit->installEventFilter(this);

  setFixedSize(minimumSizeHint());
}

MainWindow::~MainWindow()
{
  delete ui;
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
  if ( obj == ui->ttcEdit ||
       obj == ui->htEdit ||
       obj == ui->tvaEdit ) {

    QWidget *w = qobject_cast<QWidget *>(obj);
    if ( event->type() == QEvent::FocusIn || event->type() == QEvent::FocusOut ) {
        QPalette p(w->palette());
        QColor fg = p.color(w->foregroundRole());
        QColor bg = p.color(w->backgroundRole());

        // invert flg,bg
        p.setColor(w->foregroundRole(), bg);
        p.setColor(w->backgroundRole(), fg);
        w->setPalette(p);
    }

    if ( event->type() == QEvent::MouseButtonPress || event->type() == QEvent::MouseButtonDblClick ) {
      w->setFocus(Qt::MouseFocusReason);
      return true;
    }

    if ( event->type() == QEvent::FocusIn || event->type() == QEvent::FocusOut ) {
      QFocusEvent *e = (QFocusEvent *) event;
      if ( e->reason() != Qt::ActiveWindowFocusReason )
        replace = true;
    }

    if ( event->type() == QEvent::ContextMenu ) {
      return true;
    }

    if ( event->type() == QEvent::KeyPress ) {
      QLabel *l = qobject_cast<QLabel *>(w);
      const QString t = l->text();
      QKeyEvent *e = (QKeyEvent *) event;
      if ( e->key() == Qt::Key_1 ||
           e->key() == Qt::Key_2 ||
           e->key() == Qt::Key_3 ||
           e->key() == Qt::Key_4 ||
           e->key() == Qt::Key_5 ||
           e->key() == Qt::Key_6 ||
           e->key() == Qt::Key_7 ||
           e->key() == Qt::Key_8 ||
           e->key() == Qt::Key_9 ||
           e->key() == Qt::Key_0 ) {
        if ( replace ) {
          l->setText(e->text());
          replace = false;
        } else {
          l->setText(l->text()+e->text());
        }
        slot_calc();
        return true;
      }

      if ( e->key() == Qt::Key_Backspace ) {
        if ( replace ) {
          l->setText("0,0");
          replace = false;
        } else {
          if ( t.length() > 1 ) {
            l->setText(l->text().left(l->text().length()-1));
            if ( l->text().endsWith(',') ) {
              l->setText(l->text().left(l->text().length()-1));
            }
            bool ok;
            qreal val = l->text().replace(',','.').toDouble(&ok);
            if ( ok && qFuzzyIsNull(val) ) {
              l->setText("0,0");
              replace = true;
            }
          } else {
            l->setText("0,0");
            replace = true;
          }
        }
        slot_calc();
        return true;
      }

      if ( e->key() == Qt::Key_Comma || e->key() == Qt::Key_Period ) {
        if ( !t.contains('.') && !t.contains(',') ) {
          l->setText(t+',');
        }
        slot_calc();
        return true;
      }

      if ( e->key() == Qt::Key_Escape ) {
        l->setText("0,0");
        replace = true;
        slot_calc();
        return true;
      }

      if ( e->key() == Qt::Key_PageDown ) {
        auto g = ui->_20Radio->group();
        auto l = g->buttons();
        auto idx = l.indexOf(g->checkedButton());
        auto next = l[(idx+1)%l.size()];
        next->setChecked(true);
        slot_calc();
        return true;
      }

      if ( e->key() == Qt::Key_PageUp ) {
        auto g = ui->_20Radio->group();
        auto l = g->buttons();
        auto idx = l.indexOf(g->checkedButton());
        auto next = l[(idx+l.size()-1)%l.size()];
        next->setChecked(true);
        slot_calc();
        return true;
      }
    }
  }

  return QMainWindow::eventFilter(obj,event);
}

void MainWindow::slot_calc()
{
  QLabel *origin = qobject_cast<QLabel *>(qApp->focusWidget());
  if ( !origin )
    return;

  bool ok;
  qreal val = origin->text().replace(',','.').toDouble(&ok);
  if ( !ok )
    return;

  qreal taux = 0.2;
  if ( ui->_20Radio->isChecked() )
    taux = 0.2;
  if ( ui->_10Radio->isChecked() )
    taux = 0.1;
  if ( ui->_55Radio->isChecked() )
    taux = 0.055;

  qreal ht,ttc;

  if ( origin == ui->ttcEdit ) {
    ttc = val;
    // TTC -> HT
    ht = ttc/(1+taux);
    if ( qFuzzyIsNull(ht) )
      ui->htEdit->setText("0,0");
    else
      ui->htEdit->setText(QString("%1").arg(ht,0,'f',2).replace('.',','));
    if ( qFuzzyIsNull(ttc-ht) )
      ui->tvaEdit->setText("0,0");
    else
      ui->tvaEdit->setText(QString("%1").arg(ttc-ht, 0,'f',2).replace('.',','));
  }
  if ( origin == ui->htEdit ) {
    ht = val;
    // HT -> TTC
    ttc = ht*(1+taux);
    if ( qFuzzyIsNull(ttc) )
      ui->ttcEdit->setText("0,0");
    else
      ui->ttcEdit->setText(QString("%1").arg(ttc,0,'f',2).replace('.',','));
    if ( qFuzzyIsNull(ttc-ht) )
      ui->tvaEdit->setText("0,0");
    else
      ui->tvaEdit->setText(QString("%1").arg(ttc-ht, 0,'f',2).replace('.',','));
  }
}
