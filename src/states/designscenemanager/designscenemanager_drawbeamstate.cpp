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

#include "designscenemanager_drawbeamstate.h"

#include <draft/include/drawingscenetools_drawbeam.h>
#include <views/include/elev_drawingboard.h>
#include <models/include/world_drawingboard.h>
#include <ui/include/windows_mainwindow.h>
#include <actions/include/drawcommands_drawbeamaction.h>

#include <QtGlobal>
#include <QtDebug>
#include <QState>
#include <QStateMachine>
#include <QSignalTransition>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QMenu>

#include <exception>

namespace Ui = ui::windows;
namespace Draft = draft::drawingscenetools;
namespace Views = views::elev;
namespace Models = models::world;
namespace Actions = actions::drawcommands;

	namespace /* unit */	{

	/*********************************************
	*
	* constants
	*
	**********************************************/


	/*********************************************
	*
	* types
	*
	**********************************************/

	}	// namespace unit

	namespace states		{
		namespace designscenemanager		{

	/////////////helper classes//////////////////

	/*********************************************
	*
	* interface designscenemanager_DrawBeamStateImpl01
	*
	**********************************************/

	class designscenemanager_DrawBeamState::designscenemanager_DrawBeamStateImpl01		{
		// data
		private:
			designscenemanager_DrawBeamState* m_pDrawBeamState;

            threads::registry::registry_MFCommandLineSender::type m_PyThreadAddress;
            threads::registry::registry_MFAlgsSender::type m_AlgsThreadAddress;
            threads::registry::registry_MFDistrAlgsSender::type m_DistrAlgsThreadAddress;
            threads::registry::registry_MFDbInstanceSender::type m_DbThreadAddress;
            threads::registry::registry_MFNetSender::type m_NetThreadAddress;
            threads::registry::registry_MFStdOutSender::type m_StdOutThreadAddress;
            threads::registry::registry_MFStdErrSender::type m_StdErrThreadAddress;

            Draft::drawingscenetools_DrawBeam* m_pDrawBeamDrawingTool {nullptr};
            Actions::drawcommands_DrawBeamAction* m_pDrawBeamAction {nullptr};

        // helper methods
        private:
            Actions::drawcommands_DrawBeamAction* m_pGetDrawBeamAction();
            void m_vPlugSignalsSlots();
            void m_vUnPlugSignalsSlots();
            void m_vCreateTool();
            void m_vCreateTransition();
            void m_vRegister(
                std::tuple<
                    threads::registry::registry_MFCommandLineSender::type,
                    threads::registry::registry_MFAlgsSender::type,
                    threads::registry::registry_MFDistrAlgsSender::type,
                    threads::registry::registry_MFDbInstanceSender::type,
                    threads::registry::registry_MFNetSender::type,
                    threads::registry::registry_MFStdOutSender::type,
                    threads::registry::registry_MFStdErrSender::type
                > const&
            );
	
		// constructors
		public:
			designscenemanager_DrawBeamStateImpl01() = default;
			designscenemanager_DrawBeamStateImpl01(
                designscenemanager_DrawBeamState*,
                std::tuple<
                    threads::registry::registry_MFCommandLineSender::type,
                    threads::registry::registry_MFAlgsSender::type,
                    threads::registry::registry_MFDistrAlgsSender::type,
                    threads::registry::registry_MFDbInstanceSender::type,
                    threads::registry::registry_MFNetSender::type,
                    threads::registry::registry_MFStdOutSender::type,
                    threads::registry::registry_MFStdErrSender::type
                > const&
            );
			designscenemanager_DrawBeamStateImpl01(designscenemanager_DrawBeamStateImpl01 const&) = default;
			designscenemanager_DrawBeamStateImpl01(designscenemanager_DrawBeamStateImpl01&&) = default;
			~designscenemanager_DrawBeamStateImpl01() = default;


		// operators
		public:
			designscenemanager_DrawBeamStateImpl01& operator = (designscenemanager_DrawBeamStateImpl01 const&) = default;
			designscenemanager_DrawBeamStateImpl01& operator = (designscenemanager_DrawBeamStateImpl01&&) = default;

        // operations
        public:
            void m_vOnEntry(QEvent*);
            void m_vOnExit(QEvent*);
            void m_vOnSubWindowActivated(QMdiSubWindow*);
	};

	/*********************************************
	*
	* implementation designscenemanager_DrawBeamStateImpl01	
	*
	**********************************************/
	// constructors
	designscenemanager_DrawBeamState::designscenemanager_DrawBeamStateImpl01::designscenemanager_DrawBeamStateImpl01(
        designscenemanager_DrawBeamState* ai_pDrawBeamState,
        std::tuple<
            threads::registry::registry_MFCommandLineSender::type,
            threads::registry::registry_MFAlgsSender::type,
            threads::registry::registry_MFDistrAlgsSender::type,
            threads::registry::registry_MFDbInstanceSender::type,
            threads::registry::registry_MFNetSender::type,
            threads::registry::registry_MFStdOutSender::type,
            threads::registry::registry_MFStdErrSender::type
        > const& airc_tupleThreadAddresses
    )
		:	m_pDrawBeamState(ai_pDrawBeamState)
	{
		Q_ASSERT(m_pDrawBeamState);
		Q_ASSERT(!m_pDrawBeamAction);
        Q_ASSERT(!m_pDrawBeamDrawingTool);

        m_pDrawBeamAction = m_pGetDrawBeamAction();
        Q_ASSERT(m_pDrawBeamAction);

        Q_ASSERT(m_PyThreadAddress.isNull());
        Q_ASSERT(m_AlgsThreadAddress.isNull());
        Q_ASSERT(m_DistrAlgsThreadAddress.isNull());
        Q_ASSERT(m_DbThreadAddress.isNull());
        Q_ASSERT(m_NetThreadAddress.isNull());
        Q_ASSERT(m_StdOutThreadAddress.isNull());
        Q_ASSERT(m_StdErrThreadAddress.isNull());

        m_vRegister(airc_tupleThreadAddresses);    

        Q_ASSERT(!m_PyThreadAddress.isNull());
        Q_ASSERT(!m_AlgsThreadAddress.isNull());
        Q_ASSERT(!m_DistrAlgsThreadAddress.isNull());
        Q_ASSERT(!m_DbThreadAddress.isNull());
        Q_ASSERT(!m_NetThreadAddress.isNull());
        Q_ASSERT(!m_StdOutThreadAddress.isNull());
        Q_ASSERT(!m_StdErrThreadAddress.isNull());

        m_vCreateTool();
        m_vCreateTransition();

	}

    // operations
    void designscenemanager_DrawBeamState::designscenemanager_DrawBeamStateImpl01::m_vOnEntry(QEvent* ai_pEvent)   {
		Q_ASSERT(m_pDrawBeamState);
		Q_ASSERT(m_pDrawBeamAction);
        Q_ASSERT(ai_pEvent);

        m_pDrawBeamAction->setChecked(true);
        m_vPlugSignalsSlots();

        models::world::world_DrawingBoard* l_pWorldDrawingBoard = qobject_cast<models::world::world_DrawingBoard*>(
                m_pDrawBeamState->m_pGetGraphicsScene()
            )
        ;
        Q_ASSERT(l_pWorldDrawingBoard);

        m_pDrawBeamDrawingTool->m_vSetDrawingBoard(l_pWorldDrawingBoard);
    }

    void designscenemanager_DrawBeamState::designscenemanager_DrawBeamStateImpl01::m_vOnExit(QEvent* ai_pEvent)   {
		Q_ASSERT(m_pDrawBeamState);
		Q_ASSERT(m_pDrawBeamAction);
        Q_ASSERT(m_pDrawBeamDrawingTool);
        Q_ASSERT(ai_pEvent);

        m_pDrawBeamAction->setChecked(false);

        m_pDrawBeamDrawingTool->m_vSetDrawingBoard(nullptr);

        m_vUnPlugSignalsSlots();
    }

    void designscenemanager_DrawBeamState::designscenemanager_DrawBeamStateImpl01::m_vOnSubWindowActivated(
        QMdiSubWindow* ai_pMdiSubWindow
    )    {
        Q_ASSERT(m_pDrawBeamDrawingTool);
        if (!ai_pMdiSubWindow)  {
            m_pDrawBeamState->m_vSetGraphicsScene(nullptr);
            m_pDrawBeamDrawingTool->m_vSetDrawingBoard(nullptr);
        }
        else    {
            Q_ASSERT(ai_pMdiSubWindow->widget());

            auto l_pqGraphicsTabs =
                qobject_cast<QTabWidget*>(ai_pMdiSubWindow->widget())
            ;
            Q_ASSERT(l_pqGraphicsTabs);

            Views::elev_DrawingBoard* l_pElevDrawingBoard =
                qobject_cast<Views::elev_DrawingBoard*>(l_pqGraphicsTabs->currentWidget())
            ;
            Q_ASSERT(l_pElevDrawingBoard );

            Models::world_DrawingBoard* l_pWorldDrawingBoard =
                qobject_cast<Models::world_DrawingBoard*>(l_pElevDrawingBoard->scene())
            ;
            Q_ASSERT(l_pWorldDrawingBoard );

            m_pDrawBeamState->m_vSetGraphicsScene(l_pWorldDrawingBoard);
            l_pWorldDrawingBoard->m_vSetState(m_pDrawBeamState); 
            m_pDrawBeamDrawingTool->m_vSetDrawingBoard(l_pWorldDrawingBoard);
        }
    }

    // helper functions
    void designscenemanager_DrawBeamState::designscenemanager_DrawBeamStateImpl01::m_vPlugSignalsSlots()  {
        Q_ASSERT(m_pDrawBeamState);
        Q_ASSERT(m_pDrawBeamState->mrc_qGetMainWindow().mvc_pGetMdiArea());

        QObject::connect(
            m_pDrawBeamState->mrc_qGetMainWindow().mvc_pGetMdiArea(),
            &QMdiArea::subWindowActivated,
            m_pDrawBeamState,
            &designscenemanager_DrawBeamState::slot_vOnSubWindowActivated
        );
    }

    void designscenemanager_DrawBeamState::designscenemanager_DrawBeamStateImpl01::m_vUnPlugSignalsSlots()  {
        Q_ASSERT(m_pDrawBeamState);
        Q_ASSERT(m_pDrawBeamState->mrc_qGetMainWindow().mvc_pGetMdiArea());

        QObject::disconnect(
            m_pDrawBeamState->mrc_qGetMainWindow().mvc_pGetMdiArea(),
            &QMdiArea::subWindowActivated,
            m_pDrawBeamState,
            &designscenemanager_DrawBeamState::slot_vOnSubWindowActivated
        );
    }

    void designscenemanager_DrawBeamState::designscenemanager_DrawBeamStateImpl01::m_vCreateTool()    {
		Q_ASSERT(m_pDrawBeamState);

        Q_ASSERT(!m_PyThreadAddress.isNull());
        Q_ASSERT(!m_AlgsThreadAddress.isNull());
        Q_ASSERT(!m_DistrAlgsThreadAddress.isNull());
        Q_ASSERT(!m_DbThreadAddress.isNull());
        Q_ASSERT(!m_NetThreadAddress.isNull());
        Q_ASSERT(!m_StdOutThreadAddress.isNull());
        Q_ASSERT(!m_StdErrThreadAddress.isNull());

        Q_ASSERT(m_pDrawBeamState->mc_pGetPythonMain());
        Q_ASSERT(m_pDrawBeamState->mc_pGetAlgsServer());
        Q_ASSERT(m_pDrawBeamState->mc_pGetDistrAlgsServer());
        Q_ASSERT(m_pDrawBeamState->mc_pGetDbInstance());
        Q_ASSERT(m_pDrawBeamState->mc_pGetNetServer());
        Q_ASSERT(m_pDrawBeamState->mc_pGetStdOutServer());
        Q_ASSERT(m_pDrawBeamState->mc_pGetStdErrServer());

        if (!m_pDrawBeamDrawingTool)  {
            m_pDrawBeamDrawingTool = new Draft::drawingscenetools_DrawBeam(
                std::make_tuple(
                    m_PyThreadAddress,
                    m_AlgsThreadAddress,
                    m_DistrAlgsThreadAddress,
                    m_DbThreadAddress,
                    m_NetThreadAddress,
                    m_StdOutThreadAddress,
                    m_StdErrThreadAddress
                ),
                std::make_tuple(
                    m_pDrawBeamState->mc_pGetPythonMain(),
                    m_pDrawBeamState->mc_pGetAlgsServer(),
                    m_pDrawBeamState->mc_pGetDistrAlgsServer(),
                    m_pDrawBeamState->mc_pGetDbInstance(),
                    m_pDrawBeamState->mc_pGetNetServer(),
                    m_pDrawBeamState->mc_pGetStdOutServer(),
                    m_pDrawBeamState->mc_pGetStdErrServer()
                ),
                m_pDrawBeamState
            );
            Q_ASSERT(m_pDrawBeamDrawingTool);

            m_pDrawBeamState->m_vSetDrawingTool(m_pDrawBeamDrawingTool);
        }
        Q_ASSERT(m_pDrawBeamDrawingTool);
    }

    void designscenemanager_DrawBeamState::designscenemanager_DrawBeamStateImpl01::
    m_vCreateTransition()  {
        Q_ASSERT(m_pDrawBeamState);
        Q_ASSERT(m_pDrawBeamState->parentState());
        Q_ASSERT(m_pDrawBeamAction);

        QSignalTransition* l_pDrawBeamTransition =
            m_pDrawBeamState->parentState()->addTransition(
                m_pDrawBeamAction,
                &Actions::drawcommands_DrawBeamAction::triggered,
                m_pDrawBeamState
            )
        ;
        Q_ASSERT(l_pDrawBeamTransition);

        l_pDrawBeamTransition->setTransitionType(QAbstractTransition::InternalTransition);
    }

    Actions::drawcommands_DrawBeamAction*
    designscenemanager_DrawBeamState::designscenemanager_DrawBeamStateImpl01::
    m_pGetDrawBeamAction()  {
        Q_ASSERT(m_pDrawBeamState);

        QMenu* l_pDesignMenu = m_pDrawBeamState->mrc_qGetMainWindow().mc_pGetDesignMenu();
        Q_ASSERT(l_pDesignMenu);

        QList<QAction*> l_qlpActions = l_pDesignMenu->actions();
        for (QAction* l_pqAction : l_qlpActions) {
            Q_ASSERT(l_pqAction);

            auto l_pDrawBeamAction = qobject_cast<
                    Actions::drawcommands_DrawBeamAction*
                >(l_pqAction)
            ;
            if (l_pDrawBeamAction)
                return l_pDrawBeamAction
            ;
        }

        return new Actions::drawcommands_DrawBeamAction(
                *m_pDrawBeamState->mrc_qGetMainWindow().mc_pGetDesignMenu(),
                *m_pDrawBeamState->mrc_qGetMainWindow().mc_pGetDesignToolBar(),
                m_pDrawBeamState->mrc_qGetMainWindow().mc_pGetDesignActionGroup()
            )
        ;
    }

    void designscenemanager_DrawBeamState::designscenemanager_DrawBeamStateImpl01::m_vRegister(
        std::tuple<
            threads::registry::registry_MFCommandLineSender::type,
            threads::registry::registry_MFAlgsSender::type,
            threads::registry::registry_MFDistrAlgsSender::type,
            threads::registry::registry_MFDbInstanceSender::type,
            threads::registry::registry_MFNetSender::type,
            threads::registry::registry_MFStdOutSender::type,
            threads::registry::registry_MFStdErrSender::type
        > const& airc_tupleThreadAddresses
    )   {
        auto [
            l_PyThreadAddress,
            l_AlgsThreadAddress,
            l_DistrAlgsThreadAddress,
            l_DbThreadAddress,
            l_NetThreadAddress,
            l_StdOutThreadAddress,
            l_StdErrThreadAddress
        ] = airc_tupleThreadAddresses;
        m_PyThreadAddress = l_PyThreadAddress; 
        m_AlgsThreadAddress = l_AlgsThreadAddress; 
        m_DistrAlgsThreadAddress = l_DistrAlgsThreadAddress; 
        m_DbThreadAddress = l_DbThreadAddress; 
        m_NetThreadAddress = l_NetThreadAddress; 
        m_StdOutThreadAddress = l_StdOutThreadAddress; 
        m_StdErrThreadAddress = l_StdErrThreadAddress; 

        Q_ASSERT(!m_PyThreadAddress.isNull());
        Q_ASSERT(!m_AlgsThreadAddress.isNull());
        Q_ASSERT(!m_DistrAlgsThreadAddress.isNull());
        Q_ASSERT(!m_DbThreadAddress.isNull());
        Q_ASSERT(!m_NetThreadAddress.isNull());
        Q_ASSERT(!m_StdOutThreadAddress.isNull());
        Q_ASSERT(!m_StdErrThreadAddress.isNull());
    }

/////////////designscenemanager_DrawBeamState/////////////

// constructors
designscenemanager_DrawBeamState::designscenemanager_DrawBeamState(
    QState* ai_pParent,
    std::tuple<
        threads::registry::registry_MFCommandLineSender::type,
        threads::registry::registry_MFAlgsSender::type,
        threads::registry::registry_MFDistrAlgsSender::type,
        threads::registry::registry_MFDbInstanceSender::type,
        threads::registry::registry_MFNetSender::type,
        threads::registry::registry_MFStdOutSender::type,
        threads::registry::registry_MFStdErrSender::type
    > const& airc_tupleThreadAddresses,
    std::tuple<
        python::setup::setup_Main*,
        algs::strategy::strategy_Server*,
        algs::distributive::distributive_Server*,
        db::vars::vars_DbInstance*,
        io::network::network_Server*,
        io::stdoutput::stdoutput_Server*,
        io::stderror::stderror_Server*
    > const& airc_tupleThreadStateMachineControllers,
    Ui::windows_MainWindow& aor_MainWindow
)
	:	designscenemanager_DrawingState(
            "DrawBeamSceneState",
            ai_pParent,
            airc_tupleThreadStateMachineControllers,
            aor_MainWindow
        )
{
	Q_ASSERT(!m_spDrawBeamStateImpl);

	m_spDrawBeamStateImpl.reset(
		new designscenemanager_DrawBeamStateImpl01(
            this,
            airc_tupleThreadAddresses
        )
	);

}

designscenemanager_DrawBeamState::~designscenemanager_DrawBeamState()	{}

// delegates
void designscenemanager_DrawBeamState::slot_vOnSubWindowActivated(
    QMdiSubWindow* ai_pMdiSubWindow
) {
	Q_ASSERT(m_spDrawBeamStateImpl);

    m_spDrawBeamStateImpl->m_vOnSubWindowActivated(ai_pMdiSubWindow);
}

// specs
void designscenemanager_DrawBeamState::onEntry(QEvent* ai_pEvent) {
	Q_ASSERT(m_spDrawBeamStateImpl);
    Q_ASSERT(ai_pEvent);

    try {
        designscenemanager_DrawingState::onEntry(ai_pEvent);

        m_spDrawBeamStateImpl->m_vOnEntry(ai_pEvent);

    }
    catch(std::exception const&) {
    }

    QState::onEntry(ai_pEvent);
}

void designscenemanager_DrawBeamState::onExit(QEvent* ai_pEvent) {
	Q_ASSERT(m_spDrawBeamStateImpl);
    Q_ASSERT(ai_pEvent);

    try {
        m_spDrawBeamStateImpl->m_vOnExit(ai_pEvent);

        designscenemanager_DrawingState::onExit(ai_pEvent);
    }
    catch(std::exception const&) {
    }

    QState::onExit(ai_pEvent);
}

	}	// namespace designscenemanager
}	// namespace states
