#include <MainWidget.hxx>
#include <Archiver.hxx>
#include <Selector.hxx>

#include <kiconloader.h>
#include <kdirselectdialog.h>
#include <klocale.h>
#include <klineedit.h>
#include <kurlcompletion.h>

#include <qtimer.h>
#include <qtextedit.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qprogressbar.h>

//--------------------------------------------------------------------------------

MainWidget::MainWidget(QWidget *parent)
  : MainWidgetBase(parent), selector(0)
{
  connect(startButton,  SIGNAL(clicked()), this, SLOT(startBackup()));
  connect(cancelButton, SIGNAL(clicked()), Archiver::instance, SLOT(cancel()));

  connect(Archiver::instance, SIGNAL(logging(const QString &)), log, SLOT(append(const QString &)));
  connect(Archiver::instance, SIGNAL(warning(const QString &)), warnings, SLOT(append(const QString &)));

  connect(Archiver::instance, SIGNAL(targetCapacity(KIO::filesize_t)), this, SLOT(setCapacity(KIO::filesize_t)));

  connect(Archiver::instance, SIGNAL(totalFilesChanged(int)), totalFiles, SLOT(setNum(int)));
  connect(Archiver::instance, SIGNAL(totalBytesChanged(KIO::filesize_t)), this, SLOT(updateTotalBytes()));

  connect(Archiver::instance, SIGNAL(sliceProgress(int)), progressSlice, SLOT(setProgress(int)));
  connect(Archiver::instance, SIGNAL(newSlice(int)), sliceNum, SLOT(setNum(int)));

  connect(Archiver::instance, SIGNAL(fileProgress(int)), this, SLOT(setFileProgress(int)));

  connect(folder, SIGNAL(clicked()), this, SLOT(getMediaSize()));
  connect(targetDir, SIGNAL(returnPressed(const QString &)), this, SLOT(setTargetURL(const QString &)));

  KURLCompletion *kc = new KURLCompletion(KURLCompletion::DirCompletion);
  targetDir->setCompletionObject(kc);
}

//--------------------------------------------------------------------------------

void MainWidget::startBackup()
{
  log->clear();
  warnings->clear();
  elapsed.start();
  cancelButton->setEnabled(true);
  startButton->setEnabled(false);

  QTimer timer;
  connect(&timer, SIGNAL(timeout()), this, SLOT(updateElapsed()));
  timer.start(1000);

  Archiver::instance->setTarget(targetDir->text());

  QStringList includes, excludes;
  selector->getBackupList(includes, excludes);

  Archiver::instance->createArchive(includes, excludes);

  timer.stop();
  updateElapsed();
  cancelButton->setEnabled(false);
  startButton->setEnabled(true);
}

//--------------------------------------------------------------------------------

void MainWidget::setSelector(Selector *s)
{
  setCapacity(0);
  setFileProgress(100);  // to hide file progress bar

  selector = s;
}

//--------------------------------------------------------------------------------

void MainWidget::getMediaSize()
{
  KURL url = KDirSelectDialog::selectDirectory("/", false, this);

  if ( url.isEmpty() ) return;  // cancelled

  targetDir->setText(KURL_pathOrURL(url));
  Archiver::instance->setTarget(targetDir->text());
}

//--------------------------------------------------------------------------------

void MainWidget::updateElapsed()
{
  elapsedTime->setText(KGlobal::locale()->formatTime(QTime().addMSecs(elapsed.elapsed()), true, true));
}

//--------------------------------------------------------------------------------

void MainWidget::setTargetURL(const QString &url)
{
  targetDir->setText(url);
  Archiver::instance->setTarget(targetDir->text());
}

//--------------------------------------------------------------------------------

void MainWidget::updateTotalBytes()
{
  // don't use KIO::convertSize() as this would not show good progress
  // after reaching 1 GB; always show MBs
  totalSize->setText(
    QString::number(Archiver::instance->getTotalBytes() / 1024.0 / 1024.0, 'f', 2));
}

//--------------------------------------------------------------------------------

void MainWidget::setFileProgress( int percent )
{
  if ( percent == 100 )
  {
    fileProgressLabel->hide();
    fileProgress->hide();
  }
  else
  {
    fileProgressLabel->show();
    fileProgress->show();
    fileProgress->setProgress(percent);
  }
}

//--------------------------------------------------------------------------------

void MainWidget::setCapacity(KIO::filesize_t bytes)
{
  if ( bytes == 0 )
    capacity->setText(i18n("unlimited"));
  else
  {
    QString txt = KIO::convertSize(bytes);
    if ( Archiver::instance->getMaxSliceMBs() != Archiver::UNLIMITED )
      txt += " (*)";
    capacity->setText(txt);
  }
}

//--------------------------------------------------------------------------------
