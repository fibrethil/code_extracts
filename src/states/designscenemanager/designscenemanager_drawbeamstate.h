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

#ifndef INCLUDED_DESIGNSCENEMANAGER_DRAWBEAMSTATE_H
#define INCLUDED_DESIGNSCENEMANAGER_DRAWBEAMSTATE_H

#ifndef INCLUDED_STATES_EXPORT_H
#include <states/export.h>
#define INCLUDED_STATES_EXPORT_H
#endif

#ifndef INCLUDED_SETUP_MAIN_H
#include <python/include/setup_main.h>
#define INCLUDED_SETUP_MAIN_H
#endif

#ifndef INCLUDED_STRATEGY_SERVER_H
#include <algs/include/strategy_server.h>
#define INCLUDED_STRATEGY_SERVER_H
#endif

#ifndef INCLUDED_DISTRIBUTIVE_SERVER_H
#include <algs/include/distributive_server.h>
#define INCLUDED_DISTRIBUTIVE_SERVER_H
#endif

#ifndef INCLUDED_VARS_DBINSTANCE_H
#include <db/include/vars_dbinstance.h>
#define INCLUDED_VARS_DBINSTANCE_H
#endif

#ifndef INCLUDED_NETWORK_SERVER_H
#include <io/include/network_server.h>
#define INCLUDED_NETWORK_SERVER_H
#endif

#ifndef INCLUDED_STDOUTPUT_SERVER_H
#include <io/include/stdoutput_server.h>
#define INCLUDED_STDOUTPUT_SERVER_H
#endif

#ifndef INCLUDED_STDERROR_SERVER_H
#include <io/include/stderror_server.h>
#define INCLUDED_STDERROR_SERVER_H
#endif

#ifndef INCLUDED_REGISTRY_MFCOMMANDLINESENDER_H
#include <threads/include/registry_mfcommandlinesender.hpp>
#define INCLUDED_REGISTRY_MFCOMMANDLINESENDER_H
#endif

#ifndef INCLUDED_REGISTRY_MFALGSSENDER_H
#include <threads/include/registry_mfalgssender.hpp>
#define INCLUDED_REGISTRY_MFALGSSENDER_H
#endif

#ifndef INCLUDED_REGISTRY_MFDISTRALGSSENDER_H
#include <threads/include/registry_mfdistralgssender.hpp>
#define INCLUDED_REGISTRY_MFDISTRALGSSSENDER_H
#endif

#ifndef INCLUDED_REGISTRY_MFDBINSTANCESENDER_H
#include <threads/include/registry_mfdbinstancesender.hpp>
#define INCLUDED_REGISTRY_MFDBINSTANCESENDER_H
#endif

#ifndef INCLUDED_REGISTRY_MFNETSENDER_H
#include <threads/include/registry_mfnetsender.hpp>
#define INCLUDED_REGISTRY_MFNETSENDER_H
#endif

#ifndef INCLUDED_REGISTRY_MFSTDOUTSENDER_H
#include <threads/include/registry_mfstdoutsender.hpp>
#define INCLUDED_REGISTRY_MFSTDOUTSENDER_H
#endif

#ifndef INCLUDED_REGISTRY_MFSTDERRSENDER_H
#include <threads/include/registry_mfstderrsender.hpp>
#define INCLUDED_REGISTRY_MFSTDERRSENDER_H
#endif

#ifndef INCLUDED_DESIGNSCENEMANAGER_DRAWINGSTATE_H
#include "designscenemanager_drawingstate.h"
#define INCLUDED_DESIGNSCENEMANAGER_DRAWINGSTATE_H
#endif

#ifndef INCLUDED_QSCOPEDPOINTER_HPP
#include <QScopedPointer>
#define INCLUDED_QSCOPEDPOINTER_HPP
#endif

#ifndef INCLUDED_QOBJECT_H
#include <QObject>
#define INCLUDED_QOBJECT_H
#endif

class QState;
class QMdiSubWindow;

namespace ui    {
    namespace windows   {
class windows_MainWindow;
    }   // namespace windows
}   // namespace ui

namespace states	{
	namespace designscenemanager	{

/*********************************************
*
* interface
*
**********************************************/

class STATES_LIB designscenemanager_DrawBeamState
	:	public designscenemanager_DrawingState
{
	Q_OBJECT

	// data
	private:
		class designscenemanager_DrawBeamStateImpl01;
		QScopedPointer<
			designscenemanager_DrawBeamStateImpl01
		> m_spDrawBeamStateImpl;

    // specs
    private:
        virtual void onEntry(QEvent*)  override;
        virtual void onExit(QEvent*)  override;

	// constructors
	public:
		designscenemanager_DrawBeamState(
            QState* ai_pParent,
            std::tuple<
                threads::registry::registry_MFCommandLineSender::type,
                threads::registry::registry_MFAlgsSender::type,
                threads::registry::registry_MFDistrAlgsSender::type,
                threads::registry::registry_MFDbInstanceSender::type,
                threads::registry::registry_MFNetSender::type,
                threads::registry::registry_MFStdOutSender::type,
                threads::registry::registry_MFStdErrSender::type
            > const&,
            std::tuple<
                python::setup::setup_Main*,
                algs::strategy::strategy_Server*,
                algs::distributive::distributive_Server*,
                db::vars::vars_DbInstance*,
                io::network::network_Server*,
                io::stdoutput::stdoutput_Server*,
                io::stderror::stderror_Server*
            > const& airc_tupleThreadStateMachineControllers,
            ui::windows::windows_MainWindow&
        );
		designscenemanager_DrawBeamState(designscenemanager_DrawBeamState const&) = delete;
		designscenemanager_DrawBeamState(designscenemanager_DrawBeamState&&) = delete;
		virtual ~designscenemanager_DrawBeamState();


	// operators
	public:
		designscenemanager_DrawBeamState& operator = (designscenemanager_DrawBeamState const&) = delete;
		designscenemanager_DrawBeamState& operator = (designscenemanager_DrawBeamState&&) = delete;

    // delegates
    public Q_SLOTS:
        void slot_vOnSubWindowActivated(QMdiSubWindow*);
	
};

	}	// namespace designscenemanager
}	// namespace states

#endif
