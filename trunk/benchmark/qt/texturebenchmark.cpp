// Qt's OpenGL texture peinting benchmark

#include <sys/time.h>

#include <QApplication>
#include <QGLWidget>
#include <QTimer>
#include <QPainter>

static const QSize WINDOW_SIZE( 800, 800 ); // somehow close to flyer's window

// test widget
class GLWidget : public QGLWidget
{
public:

	// Constructor
	GLWidget( const QGLFormat & format ) : QGLWidget( format )
	{
		setFixedSize( WINDOW_SIZE );
		_texture.load("texture.png");
		int w = _texture.width();
		int h = _texture.height();
		_big = _texture.scaled( 2*w, 2*h, Qt::IgnoreAspectRatio, Qt::SmoothTransformation );
		_small = _texture.scaled( w/2, h/2, Qt::IgnoreAspectRatio, Qt::SmoothTransformation );
		_eight = _texture.scaled( w/8, h/8, Qt::IgnoreAspectRatio, Qt::SmoothTransformation );
	}
	
protected:

	// Paints using glDrawPixles
	virtual void paintGL()
	{
		//QPainter( this );
		QImage s = convertToGLFormat( _texture );
		
		glPixelZoom( 0.125, 0.125 );
		int w = s.width() / 8;
		int h = s.height() / 8;
		int rows = height() /  h;
		int cols = width() /  w;
		start(QString("Filling %1x%2 with sprite blitted form system memory with glDrawPixels. %5 sprites blitted").arg(cols*w).arg(rows*h).arg(rows*cols));
		for( int x = 0; x < cols; x++ )
		{
			for( int y = 0; y < rows; y++ )
			{
				float rx = (1+x-cols/2)*(2.0/cols);
				float ry = (-2+y-rows/2)*(2.0/rows);
				//qDebug("rp: %f,%f", rx, ry );
				glRasterPos2f( rx, ry );
				glDrawPixels( s.width(), s.height(), GL_RGBA, GL_UNSIGNED_BYTE, s.bits() );
			}
		}
		glFlush();
		stop();
	}

	// Paints. All testing here
	virtual void paintGLClassic()
	{
		QPainter p( this );
		
		printf("==================================\n");
		// test 1. fill entire are with unscaled textures
		{
			int w = _texture.width();
			int h = _texture.height();
			int rows = height() / h;
			int cols = width() / w;
			start(QString("Filling %1x%2 with unscaled texture. %3 texture paints used").arg(cols*w).arg(rows*h).arg(rows*cols));
			
			for( int x = 0; x < cols; x++ )
			{
				for( int y = 0; y < rows; y++ )
				{
					p.drawImage( x * w, y*h, _texture );
				}
			}
			glFlush();
			stop();
		}
		
		// test 2. fill entire area with twice as big, prescaled textures
		{
			int w = _big.width();
			int h = _big.height();
			int rows = height() / (h);
			int cols = width() / (w);
			start(QString("Filling %1x%2 with prescaled big texture. %3 texture paints used").arg(cols*w).arg(rows*h).arg(rows*cols));
			
			for( int x = 0; x < cols; x++ )
			{
				for( int y = 0; y < rows; y++ )
				{
					p.drawImage( x * w, y*h, _big );
				}
			}
			
			glFlush();
			stop();
		}
		
		//test 3 - fill entire area with twice as small, prescaled textures
		{
			int w = _small.width();
			int h = _small.height();
			int rows = height() / (h);
			int cols = width() / (w);
			start(QString("Filling %1x%2 with prescaled small texture. %3 texture paints used").arg(cols*w).arg(rows*h).arg(rows*cols));
			
			for( int x = 0; x < cols; x++ )
			{
				for( int y = 0; y < rows; y++ )
				{
					p.drawImage( x * w, y*h, _small );
				}
			}
			glFlush();
			stop();
		}
		
		//test 4 - fill entire area with twice as small textures, scaling using transform
		{
			//p.fillRect( rect(), Qt::white );
			QTransform t;
			t.scale( 0.5, 0.5 );
			p.save();
			p.setTransform( t, true );
			int w = _texture.width();
			int h = _texture.height();
			int rows = 2*height() / h;
			int cols = 2*width() / w;
			
			start(QString("Filling %1x%2 with small texture using transform. %3 texture paints used").arg(cols*w/2).arg(rows*h/2).arg(rows*cols));
			
			for( int x = 0; x < cols; x++ )
			{
				for( int y = 0; y < rows; y++ )
				{
					p.drawImage( x * w, y*h, _texture );
				}
			}
			glFlush();
			stop();
			p.restore();
		}
		
		//test 5 - fill entire area with four times as small textures, scaling using transform
		{
			//p.fillRect( rect(), Qt::white );
			QTransform t;
			t.scale( 0.25, 0.25 );
			p.save();
			p.setTransform( t, true );
			int w = _texture.width();
			int h = _texture.height();
			int rows = 4*height() / h;
			int cols = 4*width() / w;
			
			start(QString("Filling %1x%2 with small texture using transform. %3 texture paints used").arg(cols*w/4).arg(rows*h/4).arg(rows*cols));
			
			for( int x = 0; x < cols; x++ )
			{
				for( int y = 0; y < rows; y++ )
				{
					p.drawImage( x * w, y*h, _texture );
				}
			}
			glFlush();
			stop();
			p.restore();
		}
		
		//test 6 - fill entire area with eight times as small textures, scaling using transform
		{
			//p.fillRect( rect(), Qt::white );
			QTransform t;
			t.scale( 0.125, 0.125 );
			p.save();
			p.setTransform( t, true );
			int w = _texture.width();
			int h = _texture.height();
			int rows = 8*height() / h;
			int cols = 8*width() / w;
			
			start(QString("Filling %1x%2 with small texture using transform. %3 texture paints used").arg(cols*w/8).arg(rows*h/8).arg(rows*cols));
			
			for( int x = 0; x < cols; x++ )
			{
				for( int y = 0; y < rows; y++ )
				{
					p.drawImage( x * w, y*h, _texture );
				}
			}
			glFlush();
			stop();
			p.restore();
		}
		
		//test 7 - fill entire area with eight times as small textures, prescaled
		{
			//p.fillRect( rect(), Qt::white );
			int w = _eight.width();
			int h = _eight.height();
			int rows = height() / h;
			int cols = width() / w;
			
			start(QString("Filling %1x%2 with small prescaled texture. %3 texture paints used").arg(cols*w).arg(rows*h).arg(rows*cols));
			
			for( int x = 0; x < cols; x++ )
			{
				for( int y = 0; y < rows; y++ )
				{
					p.drawImage( x * w, y*h, _eight );
				}
			}
			glFlush();
			stop();
		}
		
	}
	
	// retutns current time in ms
	double getms()
	{
		struct timeval tv;
		gettimeofday( &tv, NULL );
		
		return (tv.tv_sec & 0xff ) * 1e3 + tv.tv_usec * 1e-3;
	}
	
	// starts test
	void start( const QString& testName )
	{
		_testName = testName;
		_start = getms();
	}
	
	// end test
	void stop()
	{
		double time = getms() - _start;
		printf("%-80s:%.02f ms\n", qPrintable( _testName ), time );
	}
	
	double _start;
	QString _testName;
	
	QImage _texture;
	QImage _big;
	QImage _small;
	QImage _eight;
};

// main

int main( int argc, char** argv )
{
	QApplication app( argc, argv );
	
	QGLFormat fmt;
	GLWidget window( fmt );
	
	window.show();
	
	QTimer::singleShot( 1000, & window, SLOT( updateGL() ) );
	QTimer::singleShot( 2000, & window, SLOT( updateGL() ) );
	QTimer::singleShot( 3000, & window, SLOT( updateGL() ) );
	QTimer::singleShot( 5000, & app, SLOT( quit() ) );

	app.exec();
	
}

// EOF

