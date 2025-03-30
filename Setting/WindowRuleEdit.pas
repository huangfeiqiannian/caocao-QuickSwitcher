unit WindowRuleEdit;

interface

uses
  Winapi.Windows, Winapi.Messages, System.SysUtils, System.Variants, System.Classes, Vcl.Graphics,
  Vcl.Controls, Vcl.Forms, Vcl.Dialogs, Vcl.StdCtrls, Vcl.ExtCtrls;

type
  TWindowRuleEditForm = class(TForm)
    Label1: TLabel;
    Label2: TLabel;
    Label3: TLabel;
    Label4: TLabel;
    Label5: TLabel;
    Label6: TLabel;
    Label7: TLabel;
    edtName: TEdit;
    cbbAction: TComboBox;
    edtTitle: TEdit;
    edtClassName: TEdit;
    edtStyle: TEdit;
    edtExePath: TEdit;
    edtHotkeyKey: TEdit;
    Panel1: TPanel;
    btnOK: TButton;
    btnCancel: TButton;
    procedure edtHotkeyKeyKeyPress(Sender: TObject; var Key: Char);
    procedure btnOKClick(Sender: TObject);
  private
    FOldName: string;
    procedure SetData(const AName, AAction, ATitle, AClassName, AStyle, 
      AExePath, AHotkeyKey: string);
    procedure GetData(var AName, AAction, ATitle, AClassName, AStyle,
      AExePath, AHotkeyKey: string);
  public
    class function Execute(var AName, AAction, ATitle, AClassName, AStyle,
      AExePath, AHotkeyKey: string; const AOldName: string = ''): Boolean;
  end;

var
  WindowRuleEditForm: TWindowRuleEditForm;

implementation

{$R *.dfm}

procedure TWindowRuleEditForm.edtHotkeyKeyKeyPress(Sender: TObject; var Key: Char);
begin
  // 删除键
  if Key in [#8, #127] then
    Exit;
    
  // 只允许输入小写字母和数字
  if not (Key in ['a'..'z', '0'..'9']) then
  begin
    if Key in ['A'..'Z'] then
      Key := Chr(Ord(Key) + 32)  // 转换为小写
    else
      Key := #0;
  end;
end;

procedure TWindowRuleEditForm.btnOKClick(Sender: TObject);
begin
  if TrimLeft(TrimRight(edtName.Text)) = '' then
  begin
    MessageDlg('名称不能为空', mtError, [mbOK], 0);
    edtName.SetFocus;
    ModalResult := mrNone;
    Exit;
  end;
end;

class function TWindowRuleEditForm.Execute(var AName, AAction, ATitle, AClassName,
  AStyle, AExePath, AHotkeyKey: string; const AOldName: string): Boolean;
var
  Form: TWindowRuleEditForm;
begin
  Form := TWindowRuleEditForm.Create(nil);
  try
    Form.FOldName := AOldName;
    Form.SetData(AName, AAction, ATitle, AClassName, AStyle, AExePath, AHotkeyKey);
    Result := Form.ShowModal = mrOk;
    if Result then
      Form.GetData(AName, AAction, ATitle, AClassName, AStyle, AExePath, AHotkeyKey);
  finally
    Form.Free;
  end;
end;

procedure TWindowRuleEditForm.SetData(const AName, AAction, ATitle, AClassName,
  AStyle, AExePath, AHotkeyKey: string);
begin
  edtName.Text := AName;
  if cbbAction.Items.IndexOf('TopMost') < 0 then
    cbbAction.Items.Add('TopMost');
  cbbAction.ItemIndex := cbbAction.Items.IndexOf(AAction);
  if cbbAction.ItemIndex < 0 then
    cbbAction.ItemIndex := 0;
  edtTitle.Text := ATitle;
  edtClassName.Text := AClassName;
  edtStyle.Text := AStyle;
  edtExePath.Text := AExePath;
  edtHotkeyKey.Text := AHotkeyKey;
end;

procedure TWindowRuleEditForm.GetData(var AName, AAction, ATitle, AClassName,
  AStyle, AExePath, AHotkeyKey: string);
begin
  AName := edtName.Text;
  AAction := cbbAction.Text;
  ATitle := edtTitle.Text;
  AClassName := edtClassName.Text;
  AStyle := edtStyle.Text;
  AExePath := edtExePath.Text;
  AHotkeyKey := edtHotkeyKey.Text;
end;

end. 