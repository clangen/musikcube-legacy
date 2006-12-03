#ifndef __C_ENTER_EDIT__
#define __C_ENTER_EDIT__

extern int WM_ENTEREDIT_ENTER;
extern int WM_ENTEREDIT_ESC;

class CEnterEdit : public CEdit
{
public:

    CEnterEdit( CWnd* parent );
    virtual ~CEnterEdit();

#ifdef COMPILE_STANDALONE
    virtual BOOL PreTranslateMessage(MSG* pMsg);
#else
    afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
#endif

protected:

    afx_msg void OnKillfocus();
    CWnd* m_Parent;
    DECLARE_MESSAGE_MAP()

};

#endif 