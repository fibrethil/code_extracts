/**
 *	AutoStruct
 *
 *	Copyright (C) 2015  Christos Korras
 *
 *	This program is proprietary software: you cannot redistribute it and/or modify it.
 *
 *	This program is distributed WITHOUT ANY WARRANTY; without even the implied warranty of 
 *	FITNESS FOR A PARTICULAR PURPOSE.
 *
 *	You should have received a copy of the Licenset along with this program.
 *
 *	If not, see <http://www.autostruct.com/licenses/>.
 *
**/

#ifndef INCLUDED_FILESCENETRANSITIONS_TOSAVEFILETRANSITION_H
#define INCLUDED_FILESCENETRANSITIONS_TOSAVEFILETRANSITION_H

#ifndef INCLUDED_TRANSITIONS_EXPORT_H
#include <transitions/export.h>
#define INCLUDED_TRANSITIONS_EXPORT_H
#endif

#ifndef INCLUDED_QSCOPEDPOINTER_HPP
#include <QScopedPointer>
#define INCLUDED_QSCOPEDPOINTER_HPP
#endif

#ifndef INCLUDED_QABSTRACTTRANSITION_H
#include <QAbstractTransition>
#define INCLUDED_QABSTRACTTRANSITION_H
#endif

#ifndef INCLUDED_QOBJECT_H
#include <QObject>
#define INCLUDED_QOBJECT_H
#endif


namespace ui    {
    namespace windows   {
class windows_MainWindow;
    }   // namespace windows
}   // namespace ui

namespace specs {
    namespace statesceneinterface    {

class statesceneinterface_DrawingState;

    }   // namespace statesceneinterface
}   // namespace specs

class QMainWindow;
class QState;
class QEvent;

namespace transitions	{
	namespace filescenetransitions	{

/*********************************************
*
* interface
*
**********************************************/

class TRANSITIONS_LIB filescenetransitions_ToSaveFileTransition
	:	public QAbstractTransition
{
	Q_OBJECT

	// data
	private:
		class filescenetransitions_ToSaveFileTransitionImpl01;
		QScopedPointer<
			filescenetransitions_ToSaveFileTransitionImpl01
		> m_spToSaveFileTransitionImpl;

    // overriding functionalities
    private:
        virtual bool eventTest(QEvent* ai_pEvent)   override;
        virtual void onTransition(QEvent* ai_pEvent) override;

	// constructors
	public:
		filescenetransitions_ToSaveFileTransition(
            QState* ai_pSourceState,
            specs::statesceneinterface::statesceneinterface_DrawingState* ai_pTargetState,
            ui::windows::windows_MainWindow* ai_pMainWindow
        );
		filescenetransitions_ToSaveFileTransition(filescenetransitions_ToSaveFileTransition const&) = delete;
		filescenetransitions_ToSaveFileTransition(filescenetransitions_ToSaveFileTransition&&) = delete;
		virtual ~filescenetransitions_ToSaveFileTransition();

	// operators
	public:
		filescenetransitions_ToSaveFileTransition& operator = (filescenetransitions_ToSaveFileTransition const&) = delete;
		filescenetransitions_ToSaveFileTransition& operator = (filescenetransitions_ToSaveFileTransition&&) = delete;

	
};

	}	// namespace filescenetransitions
}	// namespace transitions

#endif
