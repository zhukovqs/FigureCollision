#include "gamewindow.h"
#include "item.h"
#include "ui_gamewindow.h"
#include <QPainter>

GameWindow::GameWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    setWindowTitle("Figures");
    qsrand (QDateTime::currentMSecsSinceEpoch());

    ui->label->hide();
    ui->label_2->hide();
    ui->label_3->hide();
    ui->label_4->hide();

    etimer = new QElapsedTimer();

    timer = new QTimer(this);
    timer->setInterval( 15 );

    connect( ui->startButton, SIGNAL(clicked(bool)), this->timer, SLOT(start()) );
    connect( ui->startButton, SIGNAL(clicked(bool)), this, SLOT(newGame()) );
    connect( ui->startButton, SIGNAL(clicked(bool)), ui->label_Elapsed_end, SLOT(hide()) );
    connect( ui->startButton, SIGNAL(clicked(bool)), ui->label_MaxItems_end, SLOT(hide()) );
    connect( ui->startButton, SIGNAL(clicked(bool)), ui->label, SLOT(hide()) );
    connect( ui->startButton, SIGNAL(clicked(bool)), ui->label_2, SLOT(hide()) );
    connect( ui->startButton, SIGNAL(clicked(bool)), ui->label_3, SLOT(show()) );
    connect( ui->startButton, SIGNAL(clicked(bool)), ui->label_4, SLOT(show()) );

    connect( this, SIGNAL(noItems()), ui->startButton, SLOT(show()) );
    connect( this, SIGNAL(noItems()), ui->label, SLOT(show()) );
    connect( this, SIGNAL(noItems()), ui->label_Elapsed_end, SLOT(show()) );
    connect( this, SIGNAL(noItems()), ui->label_MaxItems_end, SLOT(show()) );
    connect( this, SIGNAL(noItems()), ui->label_Elapsed, SLOT(hide()) );
    connect( this, SIGNAL(noItems()), ui->label_Items, SLOT(hide()) );
    connect( this, SIGNAL(noItems()), ui->label_3, SLOT(hide()) );
    connect( this, SIGNAL(noItems()), ui->label_4, SLOT(hide()) );

    connect( this->timer, SIGNAL(timeout()), this, SLOT(on_timeOut()) );



}

void GameWindow::newGame()
{
    etimer->start();
    for( int i(0); i < 15; ++i )
    {
        Item item;
        item.setRadius      ( 27 );
        item.generateRandomItem( 0, 0, this->width(), this->height(), -3, 7 );
        item.generateRandomColor();
        QPainterPath path;

            if(i < 5)
            {
                path.addRect    ( -25, -25, 50, 50 );
            }
            else if(i < 10)
            {
                QPolygon triangle;
                triangle            << QPoint( 0, 30 ) << QPoint( -25, -15 )
                                    << QPoint( 25, -15 ) << QPoint( 0, 30 );
                path.addPolygon     ( triangle );
            }
            else
            {
                path.addEllipse     ( -25, -25, 50, 50 );
            }

        item.setPath        ( path );
        items.push_back     ( item );
    }
}

void GameWindow::on_startButton_clicked()
{
    ui->startButton->hide();
    etimer->restart();
}

void GameWindow::on_timeOut()
{
    for( Item & item:items )
        {
            item.setPosition( QPointF( item.getPosition().x() + item.getVelocity().x(), item.getPosition().y() + item.getVelocity().y() ) );

            if( item.getPosition().x() < item.getRadius() )
                item.setVelocity( QPointF( fabs(item.getVelocity().x()) , item.getVelocity().y() ) );

            if( item.getPosition().y() < item.getRadius() )
                item.setVelocity( QPointF( item.getVelocity().x(),  fabs(item.getVelocity().y())) );

            if( item.getPosition().x() >= this->width() - item.getRadius() )
                item.setVelocity( QPointF( -fabs(item.getVelocity().x()) , item.getVelocity().y() ) );

            if( item.getPosition().y() >= this->height() - item.getRadius()  )
                item.setVelocity( QPointF( item.getVelocity().x(),  -fabs(item.getVelocity().y() )) );

        }

    ui->label_Elapsed->setText(timeElapsed());
    ui->label_Items->setText(QString::number(items.size()));

    update();
}

void GameWindow::mousePressEvent(QMouseEvent *event)
{
    bool hit = false; //этот флаг хранит состояние попал\не попал
    for (auto item = begin(items); item != end(items); ++item)
    {
        if( item->getPath().contains(event->pos() - item->getPosition())&&event->button() == Qt::LeftButton )
        {

            item->setColorFill(Qt::black);
            items.erase(item);
            hit = true;
            update();
            break;
        }
    }

    if( !hit && !items.empty() )
    {

        for( int i(0); i < 3; ++i )
        {
            Item item;
            item.setRadius      ( 27 );
            item.generateRandomItem( 0, 0, this->width(), this->height(), -3, 7 );
            item.generateRandomColor();
            QPainterPath path;

            if( i < 1 )
            {
                path.addRect    ( -25, -25, 50, 50 );
            }
            else if( i < 2 )
            {
                QPolygon triangle;
                triangle            << QPoint( 0, 30 ) << QPoint( -25, -15 )
                                    << QPoint( 25, -15 ) << QPoint( 0, 30 );
                path.addPolygon     ( triangle );
            }
            else
            {
                path.addEllipse     ( -25, -25, 50, 50 );
            }
            item.setPath        ( path );
            items.push_back     ( item );
        }

        update();
    }

    if( items.empty() )
    {
        this->noItems();
        ui->label_Elapsed_end->setText( timeElapsed() );
       // ui->label_MaxItems_end

    }
}

void GameWindow::paintEvent( QPaintEvent *event )
{
    QPainter painter( this );
    painter.setRenderHint( QPainter::Antialiasing );


    foreach( const Item & item,items )
    {
        painter.save();     // сохранили состояние пеинтера
        painter.translate   ( item.getPosition() );
        painter.setPen      ( { item.getColorPen(), 2 } );
        painter.fillPath    ( item.getPath(), item.getColorFill() );
        painter.drawPath    ( item.getPath() );
        painter.restore();  // вернули в прежнее состояние
    }

    Q_UNUSED( event );
}

QString GameWindow::timeElapsed()
{
    qint64 res = etimer->elapsed();
    qint64 sec = res / 1000;
    qint64 min = sec / 60;
    sec %= 60;
    min %= 60;
    QString timeElapsed = QString("%1:%2").arg(min, 2, 10, QChar('0')).arg(sec, 2, 10, QChar('0'));
    return timeElapsed;
}

GameWindow::~GameWindow()
{
    delete ui;
}





