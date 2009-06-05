/*
* Copyright (c) 2003-2009 Rony Shapiro <ronys@users.sourceforge.net>.
* All rights reserved. Use of the code is allowed under the
* Artistic License 2.0 terms, as specified in the LICENSE file
* distributed with this code, or available from
* http://www.opensource.org/licenses/artistic-license-2.0.php
*/
// AddEdit_PasswordPolicy.cpp : implementation file
//

#include "stdafx.h"
#include "passwordsafe.h"
#include "ThisMfcApp.h"    // For Help
#include "DboxMain.h"
#include "AddEdit_PasswordPolicy.h"
#include "AddEdit_PropertySheet.h"

#include "corelib/PwsPlatform.h"
#include "corelib/ItemData.h"
#include "corelib/PWSprefs.h"

#if defined(POCKET_PC)
#include "pocketpc/resource.h"
#else
#include "resource.h"
#include "resource3.h"  // String resources
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAddEdit_PasswordPolicy property page

IMPLEMENT_DYNAMIC(CAddEdit_PasswordPolicy, CAddEdit_PropertyPage)

const UINT CAddEdit_PasswordPolicy::nonHex[CAddEdit_PasswordPolicy::N_NOHEX] = {
  IDC_USELOWERCASE, IDC_USEUPPERCASE, IDC_USEDIGITS,
  IDC_USESYMBOLS, IDC_EASYVISION, IDC_PRONOUNCEABLE};

const UINT CAddEdit_PasswordPolicy::LenTxts[CAddEdit_PasswordPolicy::N_HEX_LENGTHS * 2] = {
  IDC_STATIC_LC1, IDC_STATIC_LC2, IDC_STATIC_UC1, IDC_STATIC_UC2,
  IDC_STATIC_DG1, IDC_STATIC_DG2, IDC_STATIC_SY1, IDC_STATIC_SY2};

const UINT CAddEdit_PasswordPolicy::nonHexLengths[CAddEdit_PasswordPolicy::N_HEX_LENGTHS] = {
 IDC_MINLOWERLENGTH, IDC_MINUPPERLENGTH, IDC_MINDIGITLENGTH, IDC_MINSYMBOLLENGTH};

const UINT CAddEdit_PasswordPolicy::nonHexLengthSpins[CAddEdit_PasswordPolicy::N_HEX_LENGTHS] = {
  IDC_SPINLOWERCASE, IDC_SPINUPPERCASE, IDC_SPINDIGITS, IDC_SPINSYMBOLS};


CAddEdit_PasswordPolicy::CAddEdit_PasswordPolicy(CWnd *pParent, st_AE_master_data *pAEMD)
  : CAddEdit_PropertyPage(pParent,CAddEdit_PasswordPolicy::IDD, pAEMD)
{
}

CAddEdit_PasswordPolicy::~CAddEdit_PasswordPolicy()
{
}

void CAddEdit_PasswordPolicy::DoDataExchange(CDataExchange* pDX)
{
  CPropertyPage::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CAddEdit_PasswordPolicy)
  DDX_Radio(pDX, IDC_USEDEFAULTPWPOLICY, M_ipolicy());

  DDX_Text(pDX, IDC_DEFPWLENGTH, m_pwdefaultlength);
  DDX_Text(pDX, IDC_MINDIGITLENGTH, m_pwdigitminlength);
  DDX_Text(pDX, IDC_MINLOWERLENGTH, m_pwlowerminlength);
  DDX_Text(pDX, IDC_MINSYMBOLLENGTH, m_pwsymbolminlength);
  DDX_Text(pDX, IDC_MINUPPERLENGTH, m_pwupperminlength);

  DDX_Check(pDX, IDC_USELOWERCASE, m_pwuselowercase);
  DDX_Check(pDX, IDC_USEUPPERCASE, m_pwuseuppercase);
  DDX_Check(pDX, IDC_USEDIGITS, m_pwusedigits);
  DDX_Check(pDX, IDC_USESYMBOLS, m_pwusesymbols);
  DDX_Check(pDX, IDC_EASYVISION, m_pweasyvision);
  DDX_Check(pDX, IDC_USEHEXDIGITS, m_pwusehexdigits);
  DDX_Check(pDX, IDC_PRONOUNCEABLE, m_pwmakepronounceable);
  //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAddEdit_PasswordPolicy, CAddEdit_PropertyPage)
  //{{AFX_MSG_MAP(CAddEdit_PasswordPolicy)
  ON_BN_CLICKED(IDC_USEHEXDIGITS, OnUseHexdigits)
  ON_BN_CLICKED(IDC_USELOWERCASE, OnUseLowerCase)
  ON_BN_CLICKED(IDC_USEUPPERCASE, OnUseUpperCase)
  ON_BN_CLICKED(IDC_USEDIGITS, OnUseDigits)
  ON_BN_CLICKED(IDC_USESYMBOLS, OnUseSymbols)
  ON_BN_CLICKED(IDC_EASYVISION, OnEasyVision)
  ON_BN_CLICKED(IDC_PRONOUNCEABLE, OnMakePronounceable)
  ON_BN_CLICKED(IDC_USEDEFAULTPWPOLICY, OnSetDefaultPWPolicy)
  ON_BN_CLICKED(IDC_ENTRYPWPOLICY, OnSetSpecificPWPolicy)
  ON_BN_CLICKED(IDC_RESETPWPOLICY, OnResetPolicy)
  // Common
  ON_MESSAGE(PSM_QUERYSIBLINGS, OnQuerySiblings)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAddEdit_PasswordPolicy message handlers

BOOL CAddEdit_PasswordPolicy::OnInitDialog()
{
  CAddEdit_PropertyPage::OnInitDialog();

  // Set up spin control relationships
  CSpinButtonCtrl* pspin;

  pspin = (CSpinButtonCtrl *)GetDlgItem(IDC_PWLENSPIN);
  pspin->SetBuddy(GetDlgItem(IDC_DEFPWLENGTH));
  pspin->SetRange(4, 1024);
  pspin->SetBase(10);
  pspin->SetPos(m_pwdefaultlength);

  pspin = (CSpinButtonCtrl *)GetDlgItem(IDC_SPINDIGITS);
  pspin->SetBuddy(GetDlgItem(IDC_MINDIGITLENGTH));
  pspin->SetRange(0, 1024);
  pspin->SetBase(10);
  pspin->SetPos(m_pwdigitminlength);

  pspin = (CSpinButtonCtrl *)GetDlgItem(IDC_SPINLOWERCASE);
  pspin->SetBuddy(GetDlgItem(IDC_MINLOWERLENGTH));
  pspin->SetRange(0, 1024);
  pspin->SetBase(10);
  pspin->SetPos(m_pwlowerminlength);

  pspin = (CSpinButtonCtrl *)GetDlgItem(IDC_SPINSYMBOLS);
  pspin->SetBuddy(GetDlgItem(IDC_MINSYMBOLLENGTH));
  pspin->SetRange(0, 1024);
  pspin->SetBase(10);
  pspin->SetPos(m_pwsymbolminlength);

  pspin = (CSpinButtonCtrl *)GetDlgItem(IDC_SPINUPPERCASE);
  pspin->SetBuddy(GetDlgItem(IDC_MINUPPERLENGTH));
  pspin->SetRange(0, 1024);
  pspin->SetBase(10);
  pspin->SetPos(m_pwupperminlength);

  // We are given the Policy - set Dialog variables
  SetVariablesFromPolicy();

  // Disable controls based on m_ipolicy
  SetPolicyControls();

  // Set up values
  do_hex(m_pwusehexdigits == TRUE);
  do_easyorpronounceable(m_pweasyvision == TRUE || m_pwmakepronounceable == TRUE);

  UINT uiChk = (M_ipolicy() == CAddEdit_PropertySheet::DEFAULT_POLICY) ?
                IDC_USEDEFAULTPWPOLICY : IDC_ENTRYPWPOLICY;
  ((CButton *)GetDlgItem(uiChk))->SetCheck(BST_CHECKED);

  if (M_uicaller() == IDS_VIEWENTRY) {
    // Disable Buttons not already disabled in SetPolicyControls
    GetDlgItem(IDC_USEDEFAULTPWPOLICY)->EnableWindow(FALSE);
    GetDlgItem(IDC_ENTRYPWPOLICY)->EnableWindow(FALSE);
  }

  return TRUE;
}

BOOL CAddEdit_PasswordPolicy::OnKillActive()
{
  CAddEdit_PropertyPage::OnKillActive();

  CWnd *pFocus(NULL);

  // Check that options, as set, are valid.
  if (m_pwusehexdigits &&
     (m_pwuselowercase || m_pwuseuppercase || m_pwusedigits ||
      m_pwusesymbols   || m_pweasyvision   || m_pwmakepronounceable)) {
    AfxMessageBox(IDS_HEXMUTUALLYEXCL);
    pFocus = GetDlgItem(IDC_USEHEXDIGITS);
    goto error;
  }

  if (m_pwusehexdigits && (m_pwdefaultlength % 2 != 0)) {
    AfxMessageBox(IDS_HEXMUSTBEEVEN);
    pFocus = GetDlgItem(IDC_DEFPWLENGTH);
    goto error;
  }

  if (!m_pwuselowercase && !m_pwuseuppercase &&
      !m_pwusedigits    && !m_pwusesymbols   && !m_pwusehexdigits) {
    AfxMessageBox(IDS_MUSTHAVEONEOPTION);
    goto error;
  }

  if ((m_pwdefaultlength < 4) || (m_pwdefaultlength > 1024)) {
    AfxMessageBox(IDS_DEFAULTPWLENGTH);
    pFocus = GetDlgItem(IDC_DEFPWLENGTH);
    goto error;
  }

  if (!(m_pwusehexdigits || m_pweasyvision || m_pwmakepronounceable) &&
      (m_pwdigitminlength + m_pwlowerminlength +
           m_pwsymbolminlength + m_pwupperminlength) > m_pwdefaultlength) {
    AfxMessageBox(IDS_DEFAULTPWLENGTHTOOSMALL);
    pFocus = GetDlgItem(IDC_DEFPWLENGTH);
    goto error;
  }

  if ((m_pwusehexdigits || m_pweasyvision || m_pwmakepronounceable))
    m_pwdigitminlength = m_pwlowerminlength =
                  m_pwsymbolminlength = m_pwupperminlength = 1;
  //End check

  SetPolicyFromVariables();

  return TRUE;

error:
  if (pFocus != NULL)
    pFocus->SetFocus();

  return FALSE;
}

LRESULT CAddEdit_PasswordPolicy::OnQuerySiblings(WPARAM wParam, LPARAM )
{
  UpdateData(TRUE);

  SetPolicyFromVariables();

  // Have any of my fields been changed?
  switch (wParam) {
    case PP_DATA_CHANGED:
      if (M_ipolicy() != M_oldipolicy() ||
         (M_ipolicy() == CAddEdit_PropertySheet::SPECIFIC_POLICY &&
          M_pwp()     != M_oldpwp()))
        return 1L;
      break;
    case PP_UPDATE_VARIABLES:
      // MUST return 0 to force all pages to get called
      // UpdateData already performed - so data up-to-date
      // Which is why this call is done!
      return 0L;
  }
  return 0L;
}

BOOL CAddEdit_PasswordPolicy::OnApply()
{
  UpdateData(TRUE);
  CWnd *pFocus(NULL);

  if (M_ipolicy() == CAddEdit_PropertySheet::DEFAULT_POLICY) {
    M_pwp().Empty();
    return CAddEdit_PropertyPage::OnApply();
  }

  if (m_pwusehexdigits) {
    if (m_pwdefaultlength % 2 != 0) {
      AfxMessageBox(IDS_HEXMUSTBEEVEN);
      pFocus = GetDlgItem(IDC_DEFPWLENGTH);
      goto error;
    }
  }
  if (!m_pwuselowercase && !m_pwuseuppercase &&
      !m_pwusedigits    && !m_pwusesymbols) {
      AfxMessageBox(IDS_MUSTHAVEONEOPTION);
      goto error;
  }

  if ((m_pwdefaultlength < 4) || (m_pwdefaultlength > 1024)) {
    AfxMessageBox(IDS_DEFAULTPWLENGTH);
    pFocus = GetDlgItem(IDC_DEFPWLENGTH);
    goto error;
  }

  if (!(m_pwusehexdigits || m_pweasyvision || m_pwmakepronounceable) &&
      (m_pwdigitminlength  + m_pwlowerminlength +
            m_pwsymbolminlength + m_pwupperminlength) > m_pwdefaultlength) {
    AfxMessageBox(IDS_DEFAULTPWLENGTHTOOSMALL);
    pFocus = GetDlgItem(IDC_DEFPWLENGTH);
    goto error;
  }

  if ((m_pwusehexdigits || m_pweasyvision || m_pwmakepronounceable))
    m_pwdigitminlength = m_pwlowerminlength =
                  m_pwsymbolminlength = m_pwupperminlength = 1;
  //End check

  SetPolicyFromVariables();

  return CAddEdit_PropertyPage::OnApply();

error:
  // Are we the current page, if not activate this page
  if (m_ae_psh->GetActivePage() != (CAddEdit_PropertyPage *)this)
    m_ae_psh->SetActivePage(this);

  if (pFocus != NULL)
    pFocus->SetFocus();

  return FALSE;
}

void CAddEdit_PasswordPolicy::do_hex(const bool bHex)
{
  // Based on the fact that:
  // BST_UNCHECKED = 0 - which is also FALSE
  // BST_CHECKED   = 1 - which is also TRUE
  CString cs_value;
  int i;
  if (bHex) { // true means enable hex, restore state
    // Disable non-hex controls
    for (i = 0; i < N_NOHEX; i++) {
      UINT id = nonHex[i];
      m_savebool[i] = ((CButton*)GetDlgItem(id))->GetCheck();
      ((CButton*)GetDlgItem(id))->SetCheck(BST_UNCHECKED);
      GetDlgItem(id)->EnableWindow(FALSE);
    }
    // Disable lengths
    for (i = 0; i < N_HEX_LENGTHS; i++) {
      UINT id = nonHexLengths[i];
      GetDlgItem(id)->EnableWindow(FALSE);
      GetDlgItem(id)->SetWindowText(_T("0"));
      GetDlgItem(nonHexLengthSpins[i])->EnableWindow(FALSE);
      GetDlgItem(LenTxts[i * 2])->EnableWindow(FALSE);
      GetDlgItem(LenTxts[i * 2 + 1])->EnableWindow(FALSE);
    }
    // Save lengths
    m_savelen[0] = m_pwlowerminlength; m_savelen[1] = m_pwupperminlength;
    m_savelen[2] = m_pwdigitminlength; m_savelen[3] = m_pwsymbolminlength;
  } else { // non-hex, save state for possible re-enable
    // Enable non-hex controls and restore checked state
    for (i = 0; i < N_NOHEX; i++) {
      UINT id = nonHex[i];
      GetDlgItem(id)->EnableWindow(TRUE);
      ((CButton*)GetDlgItem(id))->SetCheck(m_savebool[i]);
    }
    // Restore lengths
    for (i = 0; i < N_HEX_LENGTHS; i++) {
      UINT id = nonHexLengths[i];
      cs_value.Format(_T("%d"), m_savelen[i]);
      GetDlgItem(id)->SetWindowText(cs_value);
      GetDlgItem(id)->EnableWindow(m_savebool[i]);
      GetDlgItem(nonHexLengthSpins[i])->EnableWindow(m_savebool[i]);
      GetDlgItem(LenTxts[i * 2])->EnableWindow(m_savebool[i]);
      GetDlgItem(LenTxts[i * 2 + 1])->EnableWindow(m_savebool[i]);
    }
    // Restore member variables
    m_pwlowerminlength = m_savelen[0]; m_pwupperminlength = m_savelen[1];
    m_pwdigitminlength = m_savelen[2]; m_pwsymbolminlength = m_savelen[3];
  }
}

void CAddEdit_PasswordPolicy::do_easyorpronounceable(const bool bSet)
{
  // Can't have any minimum lengths set!
  if ((m_pweasyvision == TRUE  || m_pwmakepronounceable == TRUE) &&
      (m_pwdigitminlength  > 1 || m_pwlowerminlength > 1 ||
       m_pwsymbolminlength > 1 || m_pwupperminlength > 1))
    AfxMessageBox(IDS_CANTSPECIFYMINNUMBER);

  CString cs_value;
  int i;
  if (bSet) {
    // Hide lengths
    for (i = 0; i < N_HEX_LENGTHS; i++) {
      GetDlgItem(nonHexLengths[i])->ShowWindow(SW_HIDE);
      GetDlgItem(nonHexLengthSpins[i])->ShowWindow(SW_HIDE);
      GetDlgItem(LenTxts[i * 2])->ShowWindow(SW_HIDE);
      GetDlgItem(LenTxts[i * 2 + 1])->ShowWindow(SW_HIDE);
    }
    // Save lengths
    m_savelen[0] = m_pwlowerminlength; m_savelen[1] = m_pwupperminlength;
    m_savelen[2] = m_pwdigitminlength; m_savelen[3] = m_pwsymbolminlength;
  } else {
    // Show lengths
    for (i = 0; i < N_HEX_LENGTHS; i++) {
      GetDlgItem(nonHexLengths[i])->ShowWindow(SW_SHOW);
      GetDlgItem(nonHexLengthSpins[i])->ShowWindow(SW_SHOW);
      GetDlgItem(LenTxts[i * 2])->ShowWindow(SW_SHOW);
      GetDlgItem(LenTxts[i * 2 + 1])->ShowWindow(SW_SHOW);
    }
    // Restore lengths
    m_pwlowerminlength = m_savelen[0]; m_pwupperminlength = m_savelen[1];
    m_pwdigitminlength = m_savelen[2]; m_pwsymbolminlength = m_savelen[3];
  }
}

void CAddEdit_PasswordPolicy::OnUseLowerCase()
{
  UpdateData(TRUE);
  BOOL bChecked = (IsDlgButtonChecked(IDC_USELOWERCASE) == BST_CHECKED) ? TRUE : FALSE;
  int iShow = (IsDlgButtonChecked(IDC_USELOWERCASE) == BST_CHECKED) ? SW_SHOW : SW_HIDE;

  GetDlgItem(IDC_MINLOWERLENGTH)->EnableWindow(bChecked);
  GetDlgItem(IDC_MINLOWERLENGTH)->ShowWindow(iShow);
  GetDlgItem(IDC_SPINLOWERCASE)->EnableWindow(bChecked);
  GetDlgItem(IDC_SPINLOWERCASE)->ShowWindow(iShow);
  GetDlgItem(IDC_STATIC_LC1)->EnableWindow(bChecked);
  GetDlgItem(IDC_STATIC_LC1)->ShowWindow(iShow);
  GetDlgItem(IDC_STATIC_LC2)->EnableWindow(bChecked);
  GetDlgItem(IDC_STATIC_LC2)->ShowWindow(iShow);
  m_pwlowerminlength = bChecked;  // Based on BST_UNCHECKED/FALSE = 0 & BST_CHECKED/TRUE = 1
  UpdateData(FALSE);
}

void CAddEdit_PasswordPolicy::OnUseUpperCase()
{
  UpdateData(TRUE);
  BOOL bChecked = (IsDlgButtonChecked(IDC_USEUPPERCASE) == BST_CHECKED) ? TRUE : FALSE;
  int iShow = (IsDlgButtonChecked(IDC_USEUPPERCASE) == BST_CHECKED) ? SW_SHOW : SW_HIDE;

  GetDlgItem(IDC_MINUPPERLENGTH)->EnableWindow(bChecked);
  GetDlgItem(IDC_MINUPPERLENGTH)->ShowWindow(iShow);
  GetDlgItem(IDC_SPINUPPERCASE)->EnableWindow(bChecked);
  GetDlgItem(IDC_SPINUPPERCASE)->ShowWindow(iShow);
  GetDlgItem(IDC_STATIC_UC1)->EnableWindow(bChecked);
  GetDlgItem(IDC_STATIC_UC1)->ShowWindow(iShow);
  GetDlgItem(IDC_STATIC_UC2)->EnableWindow(bChecked);
  GetDlgItem(IDC_STATIC_UC2)->ShowWindow(iShow);
  m_pwupperminlength = bChecked;  // Based on FALSE=0 & TRUE=1
  UpdateData(FALSE);
}

void CAddEdit_PasswordPolicy::OnUseDigits()
{
  UpdateData(TRUE);
  BOOL bChecked = (IsDlgButtonChecked(IDC_USEDIGITS) == BST_CHECKED) ? TRUE : FALSE;
  int iShow = (IsDlgButtonChecked(IDC_USEDIGITS) == BST_CHECKED) ? SW_SHOW : SW_HIDE;

  GetDlgItem(IDC_MINDIGITLENGTH)->EnableWindow(bChecked);
  GetDlgItem(IDC_MINDIGITLENGTH)->ShowWindow(iShow);
  GetDlgItem(IDC_SPINDIGITS)->EnableWindow(bChecked);
  GetDlgItem(IDC_SPINDIGITS)->ShowWindow(iShow);
  GetDlgItem(IDC_STATIC_DG1)->EnableWindow(bChecked);
  GetDlgItem(IDC_STATIC_DG1)->ShowWindow(iShow);
  GetDlgItem(IDC_STATIC_DG2)->EnableWindow(bChecked);
  GetDlgItem(IDC_STATIC_DG2)->ShowWindow(iShow);
  m_pwdigitminlength = bChecked;  // Based on FALSE=0 & TRUE=1
  UpdateData(FALSE);
}

void CAddEdit_PasswordPolicy::OnUseSymbols()
{
  UpdateData(TRUE);
  BOOL bChecked = (IsDlgButtonChecked(IDC_USESYMBOLS) == BST_CHECKED) ? TRUE : FALSE;
  int iShow = (IsDlgButtonChecked(IDC_USESYMBOLS) == BST_CHECKED) ? SW_SHOW : SW_HIDE;

  GetDlgItem(IDC_MINSYMBOLLENGTH)->EnableWindow(bChecked);
  GetDlgItem(IDC_MINSYMBOLLENGTH)->ShowWindow(iShow);
  GetDlgItem(IDC_SPINSYMBOLS)->EnableWindow(bChecked);
  GetDlgItem(IDC_SPINSYMBOLS)->ShowWindow(iShow);
  GetDlgItem(IDC_STATIC_SY1)->EnableWindow(bChecked);
  GetDlgItem(IDC_STATIC_SY1)->ShowWindow(iShow);
  GetDlgItem(IDC_STATIC_SY2)->EnableWindow(bChecked);
  GetDlgItem(IDC_STATIC_SY2)->ShowWindow(iShow);
  m_pwsymbolminlength = bChecked;  // Based on FALSE=0 & TRUE=1
  UpdateData(FALSE);
}

void CAddEdit_PasswordPolicy::OnUseHexdigits()
{
  UpdateData(TRUE);
  do_hex(IsDlgButtonChecked(IDC_USEHEXDIGITS) == BST_CHECKED);
  // Do not use UpdateData(FALSE) here or
  // all the good work in "do_hex" will be undone
}

void CAddEdit_PasswordPolicy::OnEasyVision()
{
  UpdateData(TRUE);
  if (m_pweasyvision && m_pwmakepronounceable) {
    ((CButton*)GetDlgItem(IDC_EASYVISION))->SetCheck(FALSE);
    AfxMessageBox(IDS_PROVISMUTUALLYEXCL);
    m_pweasyvision = FALSE;
  }

  do_easyorpronounceable(IsDlgButtonChecked(IDC_EASYVISION) == BST_CHECKED);
  // Do not use UpdateData(FALSE) here or
  // all the good work in "do_easyorpronounceable" will be undone
}

void CAddEdit_PasswordPolicy::OnMakePronounceable()
{
  UpdateData(TRUE);
  if (m_pweasyvision && m_pwmakepronounceable) {
    ((CButton*)GetDlgItem(IDC_PRONOUNCEABLE))->SetCheck(FALSE);
    AfxMessageBox(IDS_PROVISMUTUALLYEXCL);
    m_pwmakepronounceable = FALSE;
  }

  do_easyorpronounceable(IsDlgButtonChecked(IDC_PRONOUNCEABLE) == BST_CHECKED);
  // Do not use UpdateData(FALSE) here or
  // all the good work in "do_easyorpronounceable" will be undone
}

void CAddEdit_PasswordPolicy::OnResetPolicy()
{
  M_pwp() = M_default_pwp();

  SetVariablesFromPolicy();

  // Now update dialog
  UpdateData(FALSE);

  do_hex(m_pwusehexdigits == TRUE);
  do_easyorpronounceable(m_pweasyvision == TRUE || m_pwmakepronounceable == TRUE);
}

void CAddEdit_PasswordPolicy::OnSetDefaultPWPolicy()
{
  M_ipolicy() = CAddEdit_PropertySheet::DEFAULT_POLICY;

  SetPolicyControls();
}

void CAddEdit_PasswordPolicy::OnSetSpecificPWPolicy()
{
  M_ipolicy() = CAddEdit_PropertySheet::SPECIFIC_POLICY;

  SetPolicyControls();
}

void CAddEdit_PasswordPolicy::SetPolicyControls()
{
  BOOL bEnable, bEnableLengths;
  if (M_uicaller() == IDS_VIEWENTRY) {
    bEnable = bEnableLengths = FALSE;
  } else {
    bEnable = (M_ipolicy() == CAddEdit_PropertySheet::DEFAULT_POLICY) ? FALSE : TRUE;
    bEnableLengths = ((bEnable == TRUE) &&
                      (m_pweasyvision == TRUE || m_pwmakepronounceable == TRUE)) ?
                           FALSE : TRUE;
  }

  GetDlgItem(IDC_DEFPWLENGTH)->EnableWindow(bEnable);
  GetDlgItem(IDC_PWLENSPIN)->EnableWindow(bEnable);

  GetDlgItem(IDC_MINDIGITLENGTH)->EnableWindow(bEnableLengths);
  GetDlgItem(IDC_SPINDIGITS)->EnableWindow(bEnableLengths);

  GetDlgItem(IDC_MINLOWERLENGTH)->EnableWindow(bEnableLengths);
  GetDlgItem(IDC_SPINLOWERCASE)->EnableWindow(bEnableLengths);

  GetDlgItem(IDC_MINSYMBOLLENGTH)->EnableWindow(bEnableLengths);
  GetDlgItem(IDC_SPINSYMBOLS)->EnableWindow(bEnableLengths);

  GetDlgItem(IDC_MINUPPERLENGTH)->EnableWindow(bEnableLengths);
  GetDlgItem(IDC_SPINUPPERCASE)->EnableWindow(bEnableLengths);

  GetDlgItem(IDC_USELOWERCASE)->EnableWindow(bEnable);
  GetDlgItem(IDC_USEUPPERCASE)->EnableWindow(bEnable);
  GetDlgItem(IDC_USEDIGITS)->EnableWindow(bEnable);
  GetDlgItem(IDC_USESYMBOLS)->EnableWindow(bEnable);
  GetDlgItem(IDC_EASYVISION)->EnableWindow(bEnable);
  GetDlgItem(IDC_USEHEXDIGITS)->EnableWindow(bEnable);
  GetDlgItem(IDC_PRONOUNCEABLE)->EnableWindow(bEnable);

  GetDlgItem(IDC_RESETPWPOLICY)->EnableWindow(bEnable);
  UpdateData(FALSE);

  if (m_pwuselowercase == TRUE)
    OnUseLowerCase();
  if (m_pwuseuppercase == TRUE)
    OnUseUpperCase();
  if (m_pwusedigits == TRUE)
    OnUseDigits();
  if (m_pwusesymbols == TRUE)
    OnUseSymbols();
}

void CAddEdit_PasswordPolicy::SetPolicyFromVariables()
{
  if (M_ipolicy() == CAddEdit_PropertySheet::DEFAULT_POLICY) {
    M_pwp() = M_default_pwp();
  } else {
    M_pwp().Empty();
    if (m_pwuselowercase == TRUE)
      M_pwp().flags |= PWSprefs::PWPolicyUseLowercase;
    if (m_pwuseuppercase == TRUE)
      M_pwp().flags |= PWSprefs::PWPolicyUseUppercase;
    if (m_pwusedigits == TRUE)
      M_pwp().flags |= PWSprefs::PWPolicyUseDigits;
    if (m_pwusesymbols == TRUE)
      M_pwp().flags |= PWSprefs::PWPolicyUseSymbols;
    if (m_pwusehexdigits == TRUE)
      M_pwp().flags |= PWSprefs::PWPolicyUseHexDigits;
    if (m_pweasyvision == TRUE)
      M_pwp().flags |= PWSprefs::PWPolicyUseEasyVision;
    if (m_pwmakepronounceable == TRUE)
      M_pwp().flags |= PWSprefs::PWPolicyMakePronounceable;
    M_pwp().length = m_pwdefaultlength;
    M_pwp().digitminlength = m_pwdigitminlength;
    M_pwp().lowerminlength = m_pwlowerminlength;
    M_pwp().symbolminlength = m_pwsymbolminlength;
    M_pwp().upperminlength = m_pwupperminlength;
  }
}

void CAddEdit_PasswordPolicy::SetVariablesFromPolicy()
{
  m_pwuselowercase = (M_pwp().flags & PWSprefs::PWPolicyUseLowercase) ? TRUE : FALSE;
  m_pwuseuppercase = (M_pwp().flags & PWSprefs::PWPolicyUseUppercase) ? TRUE : FALSE;
  m_pwusedigits = (M_pwp().flags & PWSprefs::PWPolicyUseDigits) ? TRUE : FALSE;
  m_pwusesymbols = (M_pwp().flags & PWSprefs::PWPolicyUseSymbols) ? TRUE : FALSE;
  m_pwusehexdigits = (M_pwp().flags & PWSprefs::PWPolicyUseHexDigits) ? TRUE : FALSE;
  m_pweasyvision = (M_pwp().flags & PWSprefs::PWPolicyUseEasyVision) ? TRUE : FALSE;
  m_pwmakepronounceable =  (M_pwp().flags & PWSprefs::PWPolicyMakePronounceable) ? TRUE : FALSE;
  m_pwdefaultlength = M_pwp().length;
  m_pwdigitminlength = M_pwp().digitminlength;
  m_pwlowerminlength = M_pwp().lowerminlength;
  m_pwsymbolminlength = M_pwp().symbolminlength;
  m_pwupperminlength = M_pwp().upperminlength;

  if (m_pwuselowercase == TRUE && m_pwlowerminlength == 0)
    m_pwlowerminlength = 1;
  if (m_pwuseuppercase == TRUE && m_pwupperminlength == 0)
    m_pwupperminlength = 1;
  if (m_pwusedigits == TRUE && m_pwdigitminlength == 0)
    m_pwdigitminlength = 1;
  if (m_pwusesymbols == TRUE && m_pwsymbolminlength == 0)
    m_pwsymbolminlength = 1;

  // Save BOOL information
  m_savebool[0] = m_pwuselowercase; m_savebool[1] = m_pwuseuppercase;
  m_savebool[2] = m_pwusedigits;    m_savebool[3] = m_pwusesymbols;
  m_savebool[4] = m_pweasyvision;   m_savebool[5] = m_pwmakepronounceable;

  // Save length information
  m_savelen[0] = m_pwlowerminlength; m_savelen[1] = m_pwupperminlength;
  m_savelen[2] = m_pwdigitminlength; m_savelen[3] = m_pwsymbolminlength;
}
