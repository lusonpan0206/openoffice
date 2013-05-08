/**************************************************************
 * 
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 * 
 *************************************************************/

#ifndef SFX_SIDEBAR_CONTROLLER_HXX
#define SFX_SIDEBAR_CONTROLLER_HXX

#include "AsynchronousCall.hxx"
#include "Context.hxx"
#include "FocusManager.hxx"
#include "Panel.hxx"
#include "ResourceManager.hxx"
#include "TabBar.hxx"

#include <vcl/menu.hxx>

#include <com/sun/star/awt/XWindowPeer.hpp>
#include <com/sun/star/beans/XPropertyChangeListener.hpp>
#include <com/sun/star/frame/XDispatch.hpp>
#include <com/sun/star/ui/XContextChangeEventListener.hpp>
#include <com/sun/star/ui/XUIElement.hpp>
#include <com/sun/star/ui/XSidebar.hpp>

#include <boost/noncopyable.hpp>
#include <boost/optional.hpp>
#include <cppuhelper/compbase4.hxx>
#include <cppuhelper/basemutex.hxx>

namespace css = ::com::sun::star;
namespace cssu = ::com::sun::star::uno;


namespace
{
    typedef ::cppu::WeakComponentImplHelper4 <
        css::ui::XContextChangeEventListener,
        css::beans::XPropertyChangeListener,
        css::ui::XSidebar,
        css::frame::XStatusListener
        > SidebarControllerInterfaceBase;
}

class SfxSplitWindow;
class FixedBitmap;

namespace sfx2 { namespace sidebar {

class ContentPanelDescriptor;
class Deck;
class DeckDescriptor;
class SidebarDockingWindow;
class TabBar;
class TabBarConfiguration;

class SidebarController
    : private ::boost::noncopyable,
      private ::cppu::BaseMutex,
      public SidebarControllerInterfaceBase
{
public:
    SidebarController(
        SidebarDockingWindow* pParentWindow,
        const cssu::Reference<css::frame::XFrame>& rxFrame);
    virtual ~SidebarController (void);

    // ui::XContextChangeEventListener
    virtual void SAL_CALL notifyContextChangeEvent (const css::ui::ContextChangeEventObject& rEvent)
        throw(cssu::RuntimeException);

    // XEventListener
    virtual void SAL_CALL disposing (const css::lang::EventObject& rEventObject)
        throw(cssu::RuntimeException);

    // beans::XPropertyChangeListener
    virtual void SAL_CALL propertyChange (const css::beans::PropertyChangeEvent& rEvent)
        throw(cssu::RuntimeException);

    // frame::XStatusListener
    virtual void SAL_CALL statusChanged (const css::frame::FeatureStateEvent& rEvent)
        throw(cssu::RuntimeException);
    
    // ui::XSidebar
    virtual void SAL_CALL requestLayout (void)
        throw(cssu::RuntimeException);
    
    void NotifyResize (void);

    void SwitchToDeck (
        const ::rtl::OUString& rsDeckId);
    void OpenThenSwitchToDeck (
        const ::rtl::OUString& rsDeckId);

    /** Show only the tab bar, not the deck.
    */
    void RequestCloseDeck (void);

    /** Open the deck area and restore the parent window to its old width.
    */
    void RequestOpenDeck (void);

    FocusManager& GetFocusManager (void);

private:
    ::boost::scoped_ptr<Deck> mpCurrentDeck;
    SidebarDockingWindow* mpParentWindow;
    ::boost::scoped_ptr<TabBar> mpTabBar;
    cssu::Reference<css::frame::XFrame> mxFrame;
    Context maCurrentContext;
    Context maRequestedContext;
    ::rtl::OUString msCurrentDeckId;
    ::rtl::OUString msCurrentDeckTitle;
    AsynchronousCall maPropertyChangeForwarder;
    AsynchronousCall maContextChangeUpdate;

    /** Two flags control whether the deck is displayed or if only the
        tab bar remains visible.
        The mbIsDeckOpen flag stores the current state while
        mbIsDeckRequestedOpen stores how this state should be.  User
        actions like clicking on the deck closer affect the
        mbIsDeckRequestedOpen.  Normally both flags have the same
        value.  A document being read-only can prevent the deck from opening.
    */
    ::boost::optional<bool> mbIsDeckRequestedOpen;
    ::boost::optional<bool> mbIsDeckOpen;
    bool mbCanDeckBeOpened;
    
    /** Before the deck is closed the sidebar width is saved into this variable,
        so that it can be restored when the deck is reopended.
    */
    sal_Int32 mnSavedSidebarWidth;
    FocusManager maFocusManager;
    cssu::Reference<css::frame::XDispatch> mxReadOnlyModeDispatch;
    bool mbIsDocumentReadOnly;
    SfxSplitWindow* mpSplitWindow;
    /** When the user moves the splitter then we remember the
        width at that time.
    */
    sal_Int32 mnWidthOnSplitterButtonDown;
    /** Control that is temporarily used as replacement for the deck
        to indicate that when the current mouse drag operation ends, the
        sidebar will only show the tab bar.
    */
    ::boost::scoped_ptr<Window> mpCloseIndicator;
        
    DECL_LINK(WindowEventHandler, VclWindowEvent*);
    /** Make maRequestedContext the current context.
    */
    void UpdateConfigurations (void);
    bool ArePanelSetsEqual (
        const SharedPanelContainer& rCurrentPanels,
        const ResourceManager::PanelContextDescriptorContainer& rRequestedPanels);
    cssu::Reference<css::ui::XUIElement> CreateUIElement (
        const cssu::Reference<css::awt::XWindowPeer>& rxWindow,
        const ::rtl::OUString& rsImplementationURL,
        const bool bWantsCanvas);
    SharedPanel CreatePanel (
        const ::rtl::OUString& rsPanelId,
        ::Window* pParentWindow,
        const bool bIsInitiallyExpanded);
    void SwitchToDeck (
        const DeckDescriptor& rDeckDescriptor,
        const Context& rContext);
    void ShowPopupMenu (
        const Rectangle& rButtonBox,
        const ::std::vector<TabBar::DeckMenuData>& rDeckSelectionData,
        const ::std::vector<TabBar::DeckMenuData>& rDeckShowData) const;
    void ShowDetailMenu (const ::rtl::OUString& rsMenuCommand) const;
    ::boost::shared_ptr<PopupMenu> CreatePopupMenu (
        const ::std::vector<TabBar::DeckMenuData>& rDeckSelectionData,
        const ::std::vector<TabBar::DeckMenuData>& rDeckShowData) const;
    DECL_LINK(OnMenuItemSelected, Menu*);
    void BroadcastPropertyChange (void);

    /** The close of the deck changes the width of the child window.
        That is only possible if there is no other docking window docked above or below the sidebar.
        Return whether the width of the child window can be modified.
    */
    bool CanModifyChildWindowWidth (void);
        
    /** Set the child window container to a new width.
        Return the old width.
    */
    sal_Int32 SetChildWindowWidth (const sal_Int32 nNewWidth);

    /** Update the icons displayed in the title bars of the deck and
        the panels.  This is called once when a deck is created and
        every time when a data change event is processed.
    */
    void UpdateTitleBarIcons (void);

    void UpdateDeckOpenState (void);
    void RestrictWidth (void);
    SfxSplitWindow* GetSplitWindow (void);
    void ProcessNewWidth (const sal_Int32 nNewWidth);
    void UpdateCloseIndicator (const bool bIsIndicatorVisible);

    /** Typically called when a panel is focused via keyboard.
        Tries to scroll the deck up or down to make the given panel
        completely visible.
    */
    void ShowPanel (const Panel& rPanel);

    Context GetCurrentContext (void) const;

    virtual void SAL_CALL disposing (void);
};


} } // end of namespace sfx2::sidebar

#endif
