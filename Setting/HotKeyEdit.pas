unit HotKeyEdit;

interface

uses
  Winapi.Windows, Winapi.Messages, System.SysUtils, System.Variants, System.Classes,
  Vcl.Graphics, Vcl.Controls, Vcl.Forms, Vcl.Dialogs, Vcl.StdCtrls, System.IniFiles;

type
  THotKeyEditForm = class(TForm)
    Label1: TLabel;
    edtName: TEdit;
    Label2: TLabel;
    edtHotKey: TEdit;
    chkFollowMouse: TCheckBox;
    btnOK: TButton;
    btnCancel: TButton;
    Label3: TLabel;
    cbbListType: TComboBox;
    lblListType: TLabel;
    procedure FormCreate(Sender: TObject);
    procedure FormShow(Sender: TObject);
    procedure btnOKClick(Sender: TObject);
  private
    FOldSection: string;  // 保存旧的节名，用于清理
    FEditingSection: string;
    FKey: string;
    FCtrl: Boolean;
    FShift: Boolean;
    FAlt: Boolean;
    FWin: Boolean;
    FOnHotKeyChanged: TNotifyEvent;
    FOriginalWndProc: TWndMethod;  // 保存原始的窗口过程
    FKeyDown: Boolean;  // 添加标志位
    procedure UpdateHotKeyText;
    procedure InitializeControls;
    procedure HotKeyWndProc(var Message: TMessage);  // 自定义窗口过程
    procedure edtHotKeyKeyDown(Sender: TObject; var Key: Word; Shift: TShiftState);
  public
    procedure LoadHotKey(const Section: string);
    property OnHotKeyChanged: TNotifyEvent read FOnHotKeyChanged write FOnHotKeyChanged;
    property EditingSection: string read FEditingSection write FEditingSection;  // 添加这个属性
  end;

implementation

{$R *.dfm}

function IsKeyDown(VKey: Integer): Boolean;
begin
  Result := (GetKeyState(VKey) and $8000) <> 0;
end;

function GetKeyText(Key: Word): string;
var
  KeyState: TKeyboardState;
  Chars: array[0..1] of Char;
begin
  if Key = 0 then Exit('');
  if Key = VK_LWIN then Exit('Win');
  if Key = VK_RWIN then Exit('Win');
  if Key = VK_RBUTTON then Exit('RButton');
  if Key = VK_MBUTTON then Exit('MButton');
  
  // 获取按键名称
  SetLength(Result, 30);
  GetKeyNameText(MapVirtualKey(Key, 0) shl 16, PChar(Result), 30);
  Result := PChar(Result);
  
  // 如果获取不到名称，尝试转换为字符
  if Result = '' then
  begin
    GetKeyboardState(KeyState);
    if ToAscii(Key, MapVirtualKey(Key, 0), KeyState, @Chars, 0) = 1 then
      Result := UpperCase(Chars[0]);
  end;
end;

procedure THotKeyEditForm.FormCreate(Sender: TObject);
begin
  inherited;  // 添加这行，确保基类的FormCreate先执行
  
  FOldSection := '';
  FEditingSection := '';
  FKey := '';
  FCtrl := False;
  FAlt := False;
  FWin := False;
  FShift := False;  // 添加这个，之前漏掉了
  FKeyDown := False;  // 添加标志位
  
  edtHotKey.Text := '请按下快捷键...';
  
  // 保存原始的窗口过程并替换为自定义的
  FOriginalWndProc := edtHotKey.WindowProc;
  edtHotKey.WindowProc := HotKeyWndProc;
end;

procedure THotKeyEditForm.InitializeControls;
begin
  // 初始化ListType下拉框
  cbbListType.Items.Clear;
  cbbListType.Items.Add('树状视图');   // ListType = 0
  cbbListType.Items.Add('经典菜单');   // ListType = 1
  // ItemIndex将在LoadHotKey中根据读取的值设置
end;

procedure THotKeyEditForm.FormShow(Sender: TObject);
begin
  InitializeControls;
  if FEditingSection <> '' then
    LoadHotKey(FEditingSection);
end;

procedure THotKeyEditForm.UpdateHotKeyText;
var
  KeyText: string;
begin
  KeyText := '';
  if FCtrl then KeyText := KeyText + 'Ctrl + ';
  if FShift then KeyText := KeyText + 'Shift + ';
  if FAlt then KeyText := KeyText + 'Alt + ';
  if FWin then KeyText := KeyText + 'Win + ';
  
  if FKey <> '' then
    KeyText := KeyText + FKey
  else if KeyText <> '' then
  begin
    // 移除末尾的 ' + '
    if KeyText.EndsWith(' + ') then
      SetLength(KeyText, Length(KeyText) - 3);
  end;
    
  if KeyText = '' then
    KeyText := '请按下快捷键...';
    
  edtHotKey.Text := '';
  edtHotKey.Text := KeyText;
end;

procedure THotKeyEditForm.LoadHotKey(const Section: string);
var
  Ini: TIniFile;
  KeyChar: string;
  ListType: Integer;  // 添加ListType变量
begin
  FOldSection := Section;  // 保存旧的节名
  FEditingSection := Section;
  
  Ini := TIniFile.Create(ExtractFilePath(Application.ExeName) + 'UiSetting.ini');
  try
    if Section <> '' then
    begin
      edtName.Text := Section.Substring(7);  // 移除 'HotKey.' 前缀
      FCtrl := Ini.ReadBool(Section, 'ctrl', False);
      FShift := Ini.ReadBool(Section, 'shift', False);
      FAlt := Ini.ReadBool(Section, 'alt', False);
      FWin := Ini.ReadBool(Section, 'win', False);
      KeyChar := Ini.ReadString(Section, 'key', '');
      if KeyChar <> '' then
        FKey := KeyChar;
      chkFollowMouse.Checked := Ini.ReadBool(Section, 'followMouse', False);
      
      // 读取ListType设置
      ListType := Ini.ReadInteger(Section, 'ListType', 0);
      cbbListType.ItemIndex := ListType;  // 设置下拉框的初始值
    end;
    UpdateHotKeyText;
  finally
    Ini.Free;
  end;
end;

procedure THotKeyEditForm.btnOKClick(Sender: TObject);
var
  Ini: TIniFile;
  NewSection, KeyChar: string;
begin
  if Trim(edtName.Text) = '' then
  begin
    ShowMessage('请输入名称！');
    edtName.SetFocus;
    Exit;
  end;
  
  if Trim(edtHotKey.Text) = '' then
  begin
    ShowMessage('请设置快捷键！');
    edtHotKey.SetFocus;
    Exit;
  end;

  NewSection := 'HotKey.' + Trim(edtName.Text);
  
  Ini := TIniFile.Create(ExtractFilePath(Application.ExeName) + 'UiSetting.ini');
  try
    // 如果节名改变了，删除旧的节
    if (FOldSection <> '') and (FOldSection <> NewSection) then
      Ini.EraseSection(FOldSection);
      
    // 写入新的设置
    Ini.WriteBool(NewSection, 'ctrl', FCtrl);
    Ini.WriteBool(NewSection, 'shift', FShift);
    Ini.WriteBool(NewSection, 'alt', FAlt);
    Ini.WriteBool(NewSection, 'win', FWin);
    Ini.WriteString(NewSection, 'key', FKey);
    Ini.WriteBool(NewSection, 'followMouse', chkFollowMouse.Checked);
    
    // 保存ListType设置
    Ini.WriteInteger(NewSection, 'ListType', cbbListType.ItemIndex);
  finally
    Ini.Free;
  end;
  
  if Assigned(FOnHotKeyChanged) then
    FOnHotKeyChanged(Self);
    
  ModalResult := mrOk;
end;

procedure THotKeyEditForm.HotKeyWndProc(var Message: TMessage);
var
  Key: Word;
  Shift: TShiftState;
  RepeatCount: Integer;
  PrevKeyState: Boolean;
begin

  // 处理键盘和鼠标消息
  case Message.Msg of
    WM_KEYDOWN, WM_SYSKEYDOWN:
      begin
        // 获取重复计数（lParam的低16位）
        RepeatCount := Message.LParam and $FFFF;
        // 获取上一个键状态（lParam的第30位）
        PrevKeyState := (Message.LParam and (1 shl 30)) <> 0;

        // 如果是重复击键或者键已经处于按下状态则忽略
        if (RepeatCount > 1) or PrevKeyState then Exit;

        Key := TWMKey(Message).CharCode;
        Shift := KeyDataToShiftState(TWMKey(Message).KeyData);
        
        // 调用被注释掉的逻辑
        FKey := '';
        FCtrl := False;
        FShift := False;
        FAlt := False;
        FWin := False;

        if ssCtrl in Shift then FCtrl := True;
        if ssShift in Shift then FShift := True;
        if ssAlt in Shift then FAlt := True;
        if (Key = VK_LWIN) or (Key = VK_RWIN) then FWin := True;

        if not (Key in [VK_CONTROL, VK_SHIFT, VK_MENU, VK_LWIN, VK_RWIN]) then
          FKey := GetKeyText(Key);

        UpdateHotKeyText;
        Message.Result := 0;
        Exit;
      end;
    WM_RBUTTONDOWN:
      begin
        Key := VK_RBUTTON;
        edtHotKeyKeyDown(Self, Key, []);
        Message.Result := 0;
        Exit;
      end;
    WM_MBUTTONDOWN:
      begin
        Key := VK_MBUTTON;
        edtHotKeyKeyDown(Self, Key, []);
        Message.Result := 0;
        Exit;
      end;
    WM_MBUTTONUP:
      begin
        FKeyDown := False;
        Message.Result := 0;
        Exit;
      end;
    WM_RBUTTONUP:
      begin
        FKeyDown := False;
        Message.Result := 0;
        Exit;
      end;
  end;

  // 其他消息交给原始窗口过程处理
  FOriginalWndProc(Message);
end;

procedure THotKeyEditForm.edtHotKeyKeyDown(Sender: TObject; var Key: Word; Shift: TShiftState);
var
  i: Integer;
  HotKey: string;
begin
  // 先清空所有状态
  FKey := '';
  FCtrl := False;
  FShift := False;
  FAlt := False;
  FWin := False;

  // 遍历检查所有可能的按键
  for i := 1 to 255 do
  begin
    if IsKeyDown(i) then
    begin
      HotKey := GetKeyText(i);

      if HotKey = 'Ctrl' then FCtrl := True
      else if HotKey = 'Shift' then FShift := True
      else if HotKey = 'Alt' then FAlt := True
      else if HotKey = 'Win' then FWin := True
      else if (FKey = '')  then FKey := HotKey;
    end;
  end;

  UpdateHotKeyText;  // 更新显示当前所有按下的键
  Key := 0; // 阻止默认处理
end;

end.