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

#ifndef SFX_SIDEBAR_ENUM_CONTEXT_HXX
#define SFX_SIDEBAR_ENUM_CONTEXT_HXX

#include "sfx2/dllapi.h"

#include <rtl/ustring.hxx>

#include <vector>


namespace sfx2 { namespace sidebar {

class SFX2_DLLPUBLIC EnumContext
{
public:
    enum Application
    {
        Application_Writer,
        Application_WriterGlobal,
        Application_WriterWeb,
        Application_WriterXML,
        Application_WriterForm,
        Application_WriterReport,
        Application_Calc,
        Application_Draw,
        Application_Impress,

        // For your convenience to avoid duplicate code in the common
        // case that Draw and Impress use identical context configurations.
        Application_DrawImpress,

        // Also for your convenience for the different variants of Writer documents.
        Application_WriterVariants,

        // Used only by deck or panel descriptors.  Matches any
        // application.
        Application_Any,

        // Use this only in special circumstances.  One might be the
        // wish to disable a deck or panel during debugging.
        Application_None,

        __LastApplicationEnum = Application_None
    };
    enum Context
    {
        Context_3DObject,
        Context_Annotation,
        Context_Auditing,
        Context_Cell,
        Context_Chart,
        Context_Draw,
        Context_DrawPage,
        Context_DrawText,
        Context_EditCell,
        Context_Form,
        Context_Frame,
        Context_Graphic,
        Context_HandoutPage,
        Context_MasterPage,
        Context_Media,
        Context_MultiObject,
        Context_NotesPage,
        Context_OLE,
        Context_OutlineText,
        Context_Pivot,
        Context_SlidesorterPage,
        Context_Table,
        Context_Text,
        Context_TextObject,

        // Default context of an application.  Do we need this?
        Context_Default,

        // Used only by deck or panel descriptors.  Matches any context.
        Context_Any,

        // Special context name that is only used when a deck would
        // otherwise be empty.
        Context_Empty,

        Context_Unknown,
        
        __LastContextEnum = Context_Unknown
    };

    EnumContext (void);
    EnumContext (
        const Application eApplication,
        const Context eContext);
    EnumContext (
        const ::rtl::OUString& rsApplicationName,
        const ::rtl::OUString& rsContextName);

    /** Return a number that encodes both the application and context
        enums.
        Use the CombinedEnumContext macro in switch() statements and comparisons.
    */
    sal_Int32 GetCombinedContext(void) const;

    /** This variant of the GetCombinedContext() method treats some
        application names as identical to each other.  Replacements
        made are:
            Draw or Impress     -> DrawImpress
            Writer or WriterWeb -> WriterAndWeb
        Use the Application_DrawImpress or Application_WriterAndWeb values in the CombinedEnumContext macro.
    */
    sal_Int32 GetCombinedContext_DI(void) const;

    const ::rtl::OUString& GetApplicationName (void) const;
    Application GetApplication (void) const;
    Application GetApplication_DI (void) const;

    const ::rtl::OUString& GetContextName (void) const;

    bool operator == (const EnumContext aOther);
    bool operator != (const EnumContext aOther);
    
    /** When two contexts are matched against each other then
        application or context name may have the wildcard value 'any'.
        In order to prefer matches without wildcards over matches with
        wildcards we introduce a integer evaluation for matches.
    */
    const static sal_Int32 NoMatch;
    const static sal_Int32 OptimalMatch;

    /** Return the numeric value that describes how good the match
        between two contexts is.
        Smaller values represent better matches.
    */
    sal_Int32 EvaluateMatch (const EnumContext& rOther) const;

    /** Return the best match against the given list of contexts.
    */
    sal_Int32 EvaluateMatch (const ::std::vector<EnumContext>& rOthers) const;

    static Application GetApplicationEnum (const ::rtl::OUString& rsApplicationName);
    static const ::rtl::OUString& GetApplicationName (const Application eApplication);

    static Context GetContextEnum (const ::rtl::OUString& rsContextName);
    static const ::rtl::OUString& GetContextName (const Context eContext);

private:
    Application meApplication;
    Context meContext;

    static void ProvideApplicationContainers (void);
    static void ProvideContextContainers (void);
    static void AddEntry (const ::rtl::OUString& rsName, const Application eApplication);
    static void AddEntry (const ::rtl::OUString& rsName, const Context eContext);
};


#define CombinedEnumContext(a,e) ((static_cast<sal_uInt16>(::sfx2::sidebar::EnumContext::a)<<16)\
        | static_cast<sal_uInt16>(::sfx2::sidebar::EnumContext::e))

} } // end of namespace sfx2::sidebar

#endif
