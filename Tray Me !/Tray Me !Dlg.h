// Tray Me !Dlg.h : header file
//

#pragma once


// CTrayMeDlg dialog
class CTrayMeDlg : public CDialog
{
// Construction
public:
	CTrayMeDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_TRAYME_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	afx_msg void OnClose();
	afx_msg void OnDestroy();


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	DECLARE_MESSAGE_MAP()

	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();

	LRESULT OnTrayNotify(WPARAM wParam,LPARAM lParam);
	LRESULT OnHotKey(WPARAM wParam, LPARAM lParam);
	bool RegisterApplicationHotKeys(void);
	bool UnRegisterApplicationHotKeys(void);
};
