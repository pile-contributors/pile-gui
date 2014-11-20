/* ========================================================================= */
/* ------------------------------------------------------------------------- */
/*!
  \file			codeeditor.cc
  \date			Mar 2012
  \author		TNick

  \brief		Contains the implementation of CodeEditor class


*//*


*/
/* ------------------------------------------------------------------------- */
/* ========================================================================= */
//
//
//
//
/*  INCLUDES    ------------------------------------------------------------ */

#include	<QDebug>
#include	<QPainter>
#include	<QTextBlock>
#include	<QSettings>

#include	"codeeditor.h"


/*  INCLUDES    ============================================================ */
//
//
//
//
/*  DEFINITIONS    --------------------------------------------------------- */


/*  DEFINITIONS    ========================================================= */
//
//
//
//
/*  DATA    ---------------------------------------------------------------- */

/*  DATA    ================================================================ */
//
//
//
//
/*  CLASS    --------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
CodeEditor::CodeEditor	( QWidget * parent ) :
	QPlainTextEdit( parent )
{

	lineNumberArea = new LineNumberArea(this);

	connect(this, SIGNAL(blockCountChanged(int)),
			this, SLOT(updateLineNumberAreaWidth(int)));
	connect(this, SIGNAL(updateRequest(QRect,int)),
			this, SLOT(updateLineNumberArea(QRect,int)));
	connect(this, SIGNAL(cursorPositionChanged()),
			this, SLOT(highlightCurrentLine()));

	QFont fn = QFont( "Courier" );
	fn.setFixedPitch( true );
	fn.setStyleHint( QFont::Courier );
	setFont( fn );
    lineNumberArea->setFont( fn );

	updateLineNumberAreaWidth(0);
	highlightCurrentLine();

}
/* ========================================================================= */

/* ------------------------------------------------------------------------- */
CodeEditor::~CodeEditor	( void )
{
	/* stub */
}
/* ========================================================================= */

/* ------------------------------------------------------------------------- */
bool CodeEditor::saveState (QSettings & stg)
{
    stg.setValue ("point_size", font().pointSize ());
    return true;
}
/* ========================================================================= */

/* ------------------------------------------------------------------------- */
bool CodeEditor::restoreState (QSettings & stg)
{

    QFont	fnt = font();
    int pt_sz = stg.value ("point_size", font().pointSize ()).toInt ();
    fnt.setPointSize (pt_sz);
    setFont (fnt);

    return true;
}
/* ========================================================================= */

/* ------------------------------------------------------------------------- */
int				CodeEditor::lineNumberAreaWidth		( void )
{
	int digits = 1;
	int max = qMax(1, blockCount());
	while (max >= 10)
	{
		max /= 10;
		++digits;
	}

	int space = 3 + fontMetrics().width(QLatin1Char('9')) * digits;

	return space;
}
/* ========================================================================= */

/* ------------------------------------------------------------------------- */
void			CodeEditor::updateLineNumberAreaWidth	(
		int /* newBlockCount */ )
{
	setViewportMargins(
				lineNumberAreaWidth(),
				0, 0, 0
				);
}
/* ========================================================================= */

/* ------------------------------------------------------------------------- */
void			CodeEditor::updateLineNumberArea		(
		const QRect &rect, int dy )
{
	if (dy)
		lineNumberArea->scroll(0, dy);
	else
		lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

	if (rect.contains(viewport()->rect()))
		updateLineNumberAreaWidth(0);
}
/* ========================================================================= */

/* ------------------------------------------------------------------------- */
void			CodeEditor::resizeEvent					( QResizeEvent *e )
{
	QPlainTextEdit::resizeEvent(e);

	QRect cr = contentsRect();
	lineNumberArea->setGeometry(
				QRect(cr.left(),
					  cr.top(),
					  lineNumberAreaWidth(),
					  cr.height())
				);
}
/* ========================================================================= */

/* ------------------------------------------------------------------------- */
void			CodeEditor::keyPressEvent				( QKeyEvent * e )
{
	if ( ( e->key() == Qt::Key_Return ) || ( e->key() == Qt::Key_Enter ) )
	{
		/* ensure same indentation */
		QTextCursor cursor      = textCursor();
		QString cur_line_text   = cursor.block().text();
		QString::ConstIterator tx_iter;
		QString::ConstIterator tx_end;
		tx_iter = cur_line_text.constBegin();
		tx_end = cur_line_text.constEnd();
		while ( tx_iter != tx_end )
		{
			if ( ( *tx_iter != ' ' ) && ( *tx_iter != '\t' ) )
			{
				break;
			}
			tx_iter++;
		}
		QString	s_ins = QString(
					cur_line_text.constBegin(),
					tx_iter - cur_line_text.constBegin()
					);
		cursor.insertText( "\n" + s_ins );

	}
	else
	{
		QPlainTextEdit::keyPressEvent( e );
	}
}
/* ========================================================================= */

/* ------------------------------------------------------------------------- */
void			CodeEditor::wheelEvent	( QWheelEvent * event )
{
	int numDegrees = event->delta() / 8;
	int numSteps = numDegrees / 15;  // see QWheelEvent documentation


	if ( ( event->modifiers() & Qt::ControlModifier ) != 0 )
	{
		QFont	fnt = font();
		fnt.setPointSize( fnt.pointSize() + numSteps );
		setFont( fnt );
	}
	else
	{
		QPlainTextEdit::wheelEvent( event );
	}
}
/* ========================================================================= */

/* ------------------------------------------------------------------------- */
void			CodeEditor::highlightCurrentLine		( void )
{
	QList<QTextEdit::ExtraSelection> extraSelections;

	if ( !isReadOnly() )
	{
		QTextEdit::ExtraSelection selection;

		QColor lineColor = QColor(Qt::yellow).lighter(160);

		selection.format.setBackground(lineColor);
		selection.format.setProperty(QTextFormat::FullWidthSelection, true);
		selection.cursor = textCursor();
		selection.cursor.clearSelection();
		extraSelections.append(selection);
	}

	setExtraSelections(extraSelections);
}
/* ========================================================================= */

/* ------------------------------------------------------------------------- */
void			CodeEditor::lineNumberAreaPaintEvent	( QPaintEvent *event )
{
	QPainter painter( lineNumberArea );
	painter.fillRect( event->rect(), QColor( 221, 234, 254 ) );

	QTextBlock block = firstVisibleBlock();
	int blockNumber = block.blockNumber();
	int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
	int bottom = top + (int) blockBoundingRect(block).height();

	while (block.isValid() && top <= event->rect().bottom())
	{
		if (block.isVisible() && bottom >= event->rect().top()) {
			QString number = QString::number(blockNumber + 1);
			painter.setPen( Qt::blue );
            painter.drawText(0, top, lineNumberArea->width(), fontMetrics().height()*2,
							 Qt::AlignRight, number);
		}

		block = block.next();
		top = bottom;
		bottom = top + (int) blockBoundingRect(block).height();
		++blockNumber;
	}
}
/* ========================================================================= */


/*  CLASS    =============================================================== */
//
//
//
//
/* ------------------------------------------------------------------------- */
/* ========================================================================= */
