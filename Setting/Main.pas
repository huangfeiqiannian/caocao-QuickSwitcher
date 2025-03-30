unit Main;

interface

uses
  Winapi.Windows, Winapi.Messages, System.SysUtils, System.Variants, System.Classes, Vcl.Graphics,
  Vcl.Controls, Vcl.Forms, Vcl.Dialogs, Vcl.ComCtrls, Vcl.StdCtrls, System.IniFiles,
  System.StrUtils, HotKeyEdit, Vcl.ExtCtrls, WindowRuleEdit, Vcl.Samples.Spin,
  ShellAPI, System.Win.Registry, Vcl.Themes;

type
  TMainForm = class(TForm)
    PageControl1: TPageControl;
    TabSheet1: TTabSheet;
    ListView1: TListView;
    Panel1: TPanel;
    btnNew: TButton;
    btnEdit: TButton;
    btnDelete: TButton;
    TabSheet2: TTabSheet;
    ListView2: TListView;
    Panel2: TPanel;
    btnNewRule: TButton;
    btnEditRule: TButton;
    btnDeleteRule: TButton;
    TabSheet3: TTabSheet;
    GroupBox1: TGroupBox;
    Label1: TLabel;
    edtMaxTitleLength: TSpinEdit;
    cbbHKType: TComboBox;
    Label2: TLabel;
    edtDefaultKeys: TEdit;
    Label3: TLabel;
    btnReload: TButton;
    btnExit: TButton;
    chkRunAsAdmin: TCheckBox;
    Label4: TLabel;
    cbbTreeTheme: TComboBox;
    procedure FormCreate(Sender: TObject);
    procedure btnNewClick(Sender: TObject);
    procedure btnEditClick(Sender: TObject);
    procedure btnDeleteClick(Sender: TObject);
    procedure ListView1DblClick(Sender: TObject);
    procedure ListView2DblClick(Sender: TObject);
    procedure btnNewRuleClick(Sender: TObject);
    procedure btnEditRuleClick(Sender: TObject);
    procedure btnDeleteRuleClick(Sender: TObject);
    procedure btnReloadClick(Sender: TObject);
    procedure btnExitClick(Sender: TObject);
    procedure chkRunAsAdminClick(Sender: TObject);
    procedure cbbTreeThemeChange(Sender: TObject);
    procedure WMSettingChange(var Message: TWMSettingChange); message WM_SETTINGCHANGE;
  private
    procedure LoadHotKeys;
    function GetHotKeyString(const Section: string; Ini: TIniFile): string;
    procedure OnHotKeyChangedHandler(Sender: TObject);
    procedure LoadWindowRules;
    procedure SaveWindowRule(const Section, Action, Title, ClassName, Style, ExePath, HotkeyKey: string);
    procedure DeleteWindowRule(const Section: string);
    procedure LoadMiscSettings;
    procedure SaveMiscSettings;
    procedure SaveMiscSettingsHandler(Sender: TObject);
    procedure UpdateRunAsAdminSetting(Enabled: Boolean);
    procedure LoadRunAsAdminSetting;
    procedure UpdateTheme;
    function IsDarkThemeEnabled: Boolean;
  public
    { Public declarations }
  end;

var
  MainForm: TMainForm;

implementation

{$R *.dfm}

procedure TMainForm.FormCreate(Sender: TObject);
begin
  // 中文
  Caption := '热键管理程序';
  // 初始化 TreeTheme 下拉框
  cbbTreeTheme.Items.Add('自动');
  cbbTreeTheme.Items.Add('亮色');
  cbbTreeTheme.Items.Add('暗色');
  cbbTreeTheme.ItemIndex := 0;

  LoadHotKeys;
  LoadWindowRules;
  LoadMiscSettings;
  
  // 应用初始主题
  UpdateTheme;
end;

function TMainForm.GetHotKeyString(const Section: string; Ini: TIniFile): string;
var
  HasCtrl, HasShift, HasAlt, HasWin: Boolean;
  KeyChar: string;
  ListType: Integer;
begin
  HasCtrl := Ini.ReadBool(Section, 'ctrl', False);
  HasShift := Ini.ReadBool(Section, 'shift', False);
  HasAlt := Ini.ReadBool(Section, 'alt', False);
  HasWin := Ini.ReadBool(Section, 'win', False);
  KeyChar := Ini.ReadString(Section, 'key', '');
  ListType := Ini.ReadInteger(Section, 'ListType', 0);

  Result := '';
  if HasCtrl then Result := Result + 'Ctrl+';
  if HasShift then Result := Result + 'Shift+';
  if HasAlt then Result := Result + 'Alt+';
  if HasWin then Result := Result + 'Win+';
  Result := Result + KeyChar;
  if Result.EndsWith('+') then
    SetLength(Result, Length(Result) - 1);

  {// 添加列表风格信息
  if ListType = 0 then
    Result := Result + ' (树状视图)'
  else
    Result := Result + ' (经典菜单)';}
end;

procedure TMainForm.LoadHotKeys;
var
  Ini: TIniFile;
  Sections: TStringList;
  i: Integer;
  Item: TListItem;
  Section: string;
  ListType: Integer;
begin
  ListView1.Items.Clear;

  Ini := TIniFile.Create(ExtractFilePath(Application.ExeName) + 'UiSetting.ini');
  Sections := TStringList.Create;
  try
    Ini.ReadSections(Sections);
    for i := 0 to Sections.Count - 1 do
    begin
      Section := Sections[i];
      if Section.StartsWith('HotKey.') then
      begin
        Item := ListView1.Items.Add;
        Item.Caption := Section.Substring(7); // 去掉 'HotKey.' 前缀
        Item.SubItems.Add(GetHotKeyString(Section, Ini));
        Item.SubItems.Add(IfThen(Ini.ReadBool(Section, 'followMouse', False), '跟随鼠标', '居中显示'));
        // 添加列表风格列
        ListType := Ini.ReadInteger(Section, 'ListType', 0);
        if ListType = 0 then
          Item.SubItems.Add('树状视图')
        else
          Item.SubItems.Add('经典菜单');
      end;
    end;
  finally
    Sections.Free;
    Ini.Free;
  end;
end;

procedure TMainForm.OnHotKeyChangedHandler(Sender: TObject);
begin
  LoadHotKeys;
end;

procedure TMainForm.btnNewClick(Sender: TObject);
var
  HotKeyEditForm: THotKeyEditForm;
begin
  HotKeyEditForm := THotKeyEditForm.Create(Self);
  try
    HotKeyEditForm.OnHotKeyChanged := OnHotKeyChangedHandler;
    HotKeyEditForm.ShowModal;
  finally
    HotKeyEditForm.Free;
  end;
end;

procedure TMainForm.btnEditClick(Sender: TObject);
var
  HotKeyEditForm: THotKeyEditForm;
begin
  if ListView1.Selected = nil then Exit;

  HotKeyEditForm := THotKeyEditForm.Create(Self);
  try
    HotKeyEditForm.OnHotKeyChanged := OnHotKeyChangedHandler;
    HotKeyEditForm.LoadHotKey('HotKey.' + ListView1.Selected.Caption);
    HotKeyEditForm.ShowModal;
  finally
    HotKeyEditForm.Free;
  end;
end;

procedure TMainForm.ListView1DblClick(Sender: TObject);
begin
  btnEdit.Click;
end;

procedure TMainForm.btnDeleteClick(Sender: TObject);
var
  Ini: TIniFile;
begin
  if ListView1.Selected = nil then Exit;

  if MessageDlg('确定要删除选中的热键吗？', mtConfirmation, [mbYes, mbNo], 0) = mrYes then
  begin
    Ini := TIniFile.Create(ExtractFilePath(Application.ExeName) + 'UiSetting.ini');
    try
      Ini.EraseSection('HotKey.' + ListView1.Selected.Caption);
    finally
      Ini.Free;
    end;
    LoadHotKeys; // 重新加载热键列表
  end;
end;

procedure TMainForm.LoadWindowRules;
var
  Ini: TIniFile;
  Sections: TStringList;
  i: Integer;
  Item: TListItem;
  Action: string;
begin
  ListView2.Items.Clear;
  Ini := TIniFile.Create(ExtractFilePath(Application.ExeName) + 'setting.ini');
  Sections := TStringList.Create;
  try
    Ini.ReadSections(Sections);
    for i := 0 to Sections.Count - 1 do
    begin
      if SameText(Sections[i], 'DefaultHotkeys') then
        Continue;

      Action := Ini.ReadString(Sections[i], 'Action', '');
      if Action <> '' then
      begin
        Item := ListView2.Items.Add;
        Item.Caption := Sections[i];
        Item.SubItems.Add(Action);
      end;
    end;
  finally
    Sections.Free;
    Ini.Free;
  end;
end;

procedure TMainForm.SaveWindowRule(const Section, Action, Title, ClassName,
  Style, ExePath, HotkeyKey: string);
var
  Ini: TIniFile;
begin
  Ini := TIniFile.Create(ExtractFilePath(Application.ExeName) + 'setting.ini');
  try
    Ini.WriteString(Section, 'Action', Action);
    if Title <> '' then
      Ini.WriteString(Section, 'Title', Title);
    if ClassName <> '' then
      Ini.WriteString(Section, 'ClassName', ClassName);
    if Style <> '' then
      Ini.WriteString(Section, 'Style', Style);
    if ExePath <> '' then
      Ini.WriteString(Section, 'ExePath', ExePath);
    if HotkeyKey <> '' then
      Ini.WriteString(Section, 'HotkeyKey', HotkeyKey);
  finally
    Ini.Free;
  end;
  LoadWindowRules;
end;

procedure TMainForm.DeleteWindowRule(const Section: string);
var
  Ini: TIniFile;
begin
  Ini := TIniFile.Create(ExtractFilePath(Application.ExeName) + 'setting.ini');
  try
    Ini.EraseSection(Section);
  finally
    Ini.Free;
  end;
  LoadWindowRules;
end;

procedure TMainForm.ListView2DblClick(Sender: TObject);
begin
  btnEditRule.Click;
end;

procedure TMainForm.btnNewRuleClick(Sender: TObject);
var
  Name, Action, Title, ClassName, Style, ExePath, HotkeyKey: string;
begin
  Name := '';
  Action := 'Ignore';
  Title := '';
  ClassName := '';
  Style := '';
  ExePath := '';
  HotkeyKey := '';

  if TWindowRuleEditForm.Execute(Name, Action, Title, ClassName, Style,
    ExePath, HotkeyKey) then
    SaveWindowRule(Name, Action, Title, ClassName, Style, ExePath, HotkeyKey);
end;

procedure TMainForm.btnEditRuleClick(Sender: TObject);
var
  Item: TListItem;
  Ini: TIniFile;
  Name, Action, Title, ClassName, Style, ExePath, HotkeyKey: string;
begin
  Item := ListView2.Selected;
  if Item = nil then
    Exit;

  Name := Item.Caption;
  Ini := TIniFile.Create(ExtractFilePath(Application.ExeName) + 'setting.ini');
  try
    Action := Ini.ReadString(Name, 'Action', 'Ignore');
    Title := Ini.ReadString(Name, 'Title', '');
    ClassName := Ini.ReadString(Name, 'ClassName', '');
    Style := Ini.ReadString(Name, 'Style', '');
    ExePath := Ini.ReadString(Name, 'ExePath', '');
    HotkeyKey := Ini.ReadString(Name, 'HotkeyKey', '');
  finally
    Ini.Free;
  end;

  if TWindowRuleEditForm.Execute(Name, Action, Title, ClassName, Style,
    ExePath, HotkeyKey, Item.Caption) then
  begin
    if not SameText(Name, Item.Caption) then
      DeleteWindowRule(Item.Caption);
    SaveWindowRule(Name, Action, Title, ClassName, Style, ExePath, HotkeyKey);
  end;
end;

procedure TMainForm.btnDeleteRuleClick(Sender: TObject);
var
  Item: TListItem;
begin
  Item := ListView2.Selected;
  if (Item = nil) or
     (MessageDlg('确定要删除选中的窗口规则吗？', mtConfirmation, [mbYes, mbNo], 0) <> mrYes) then
    Exit;

  DeleteWindowRule(Item.Caption);
end;

procedure TMainForm.LoadMiscSettings;
var
  Ini: TIniFile;
  SettingIni: TIniFile;
  Theme: string;
begin
  Ini := TIniFile.Create(ExtractFilePath(Application.ExeName) + 'UiSetting.ini');
  SettingIni := TIniFile.Create(ExtractFilePath(Application.ExeName) + 'setting.ini');
  try
    // Load MaxTitleLength
    edtMaxTitleLength.Value := Ini.ReadInteger('Settings', 'MaxTitleLength', 50);

    // Load HKType
    if Ini.ReadString('HKType', 'Type', 'HK_POLLING') = 'HK_POLLING' then
      cbbHKType.ItemIndex := 0
    else
      cbbHKType.ItemIndex := 1;

    // Load DefaultKeys
    edtDefaultKeys.Text := SettingIni.ReadString('DefaultHotkeys', 'Keys', '1234qwerasdfzxcvtyuighjkbnm');

    // Load TreeTheme
    Theme := Ini.ReadString('Settings', 'TreeTheme', 'Auto');
    if Theme = 'Light' then
      cbbTreeTheme.ItemIndex := 1
    else if Theme = 'Dark' then
      cbbTreeTheme.ItemIndex := 2
    else
      cbbTreeTheme.ItemIndex := 0;

    // Set up change events
    edtMaxTitleLength.OnExit := SaveMiscSettingsHandler;
    cbbHKType.OnChange := SaveMiscSettingsHandler;
    edtDefaultKeys.OnExit := SaveMiscSettingsHandler;
    cbbTreeTheme.OnChange := SaveMiscSettingsHandler;
  finally
    Ini.Free;
    SettingIni.Free;
  end;

  // Load RunAsAdmin setting
  LoadRunAsAdminSetting;

  // Set up change event
  chkRunAsAdmin.OnClick := chkRunAsAdminClick;
end;

procedure TMainForm.SaveMiscSettings;
var
  Ini: TIniFile;
  SettingIni: TIniFile;
begin
  Ini := TIniFile.Create(ExtractFilePath(Application.ExeName) + 'UiSetting.ini');
  SettingIni := TIniFile.Create(ExtractFilePath(Application.ExeName) + 'setting.ini');
  try
    // Save MaxTitleLength
    Ini.WriteInteger('Settings', 'MaxTitleLength', edtMaxTitleLength.Value);

    // Save HKType
    if cbbHKType.ItemIndex = 0 then
      Ini.WriteString('HKType', 'Type', 'HK_POLLING')
    else
      Ini.WriteString('HKType', 'Type', 'HK_HOOK');

    // Save DefaultKeys
    SettingIni.WriteString('DefaultHotkeys', 'Keys', edtDefaultKeys.Text);

    // Save TreeTheme
    case cbbTreeTheme.ItemIndex of
      1: Ini.WriteString('Settings', 'TreeTheme', 'Light');
      2: Ini.WriteString('Settings', 'TreeTheme', 'Dark');
    else
      Ini.WriteString('Settings', 'TreeTheme', 'Auto');
    end;
  finally
    Ini.Free;
    SettingIni.Free;
  end;
end;

procedure TMainForm.SaveMiscSettingsHandler(Sender: TObject);
begin
  SaveMiscSettings;
end;

procedure TMainForm.btnReloadClick(Sender: TObject);
var
  hWnd: THandle;
  ExePath: string;
begin
  hWnd := FindWindow('{902B2675-F285-4CBE-87C8-4D221779EC6D}', '{902B2675-F285-4CBE-87C8-4D221779EC6D}');
  if hWnd <> 0 then
  begin
    // 发送重新加载设置的消息
    PostMessage(hWnd, WM_USER + 2, 0, 0);
    // 更新当前窗口的主题
    UpdateTheme;
  end
  else
  begin
    ExePath := ExtractFilePath(Application.ExeName) + 'CCSW.EXE';
    if FileExists(ExePath) then
    begin
      // 启动新实例
      if ShellExecute(0, 'open', PChar(ExePath), nil, nil, SW_SHOWNORMAL) <= 32 then
        ShowMessage('无法启动 CCSW.EXE');
    end
    else
    begin
      ShowMessage('未找到目标窗口，且无法启动 CCSW.EXE');
    end;
  end;
end;

procedure TMainForm.btnExitClick(Sender: TObject);
begin
  Close;
end;

procedure TMainForm.UpdateRunAsAdminSetting(Enabled: Boolean);
var
  Reg: TRegistry;
  ExePath: string;
begin
  Reg := TRegistry.Create;
  try
    Reg.RootKey := HKEY_CURRENT_USER;
    if Reg.OpenKey('Software\Microsoft\Windows NT\CurrentVersion\AppCompatFlags\Layers', True) then
    begin
      ExePath := ExtractFilePath(Application.ExeName) + 'CCSW.EXE';
      if Enabled then
        Reg.WriteString(ExePath, 'RUNASADMIN')
      else
        Reg.DeleteValue(ExePath);
    end;
  finally
    Reg.Free;
  end;
end;

procedure TMainForm.LoadRunAsAdminSetting;
var
  Reg: TRegistry;
  ExePath: string;
begin
  Reg := TRegistry.Create;
  try
    Reg.RootKey := HKEY_CURRENT_USER;
    if Reg.OpenKeyReadOnly('Software\Microsoft\Windows NT\CurrentVersion\AppCompatFlags\Layers') then
    begin
      ExePath := ExtractFilePath(Application.ExeName) + 'CCSW.EXE';
      chkRunAsAdmin.Checked := Reg.ValueExists(ExePath) and
        (Reg.ReadString(ExePath) = 'RUNASADMIN');
    end;
  finally
    Reg.Free;
  end;
end;

procedure TMainForm.chkRunAsAdminClick(Sender: TObject);
begin
  UpdateRunAsAdminSetting(chkRunAsAdmin.Checked);
end;

procedure TMainForm.UpdateTheme;
var
  Ini: TIniFile;
  Theme: string;
begin
  Ini := TIniFile.Create(ExtractFilePath(Application.ExeName) + 'UiSetting.ini');
  try
    Theme := Ini.ReadString('Settings', 'TreeTheme', 'Auto');
    if (Theme = 'Dark') or ((Theme = 'Auto') and IsDarkThemeEnabled) then
      TStyleManager.TrySetStyle('Aqua Graphite')  // 暗色主题
    else
      TStyleManager.TrySetStyle('Aqua Light Slate');  // 亮色主题
  finally
    Ini.Free;
  end;
end;

procedure TMainForm.cbbTreeThemeChange(Sender: TObject);
begin
  UpdateTheme;
  SaveMiscSettings;
end;

function TMainForm.IsDarkThemeEnabled: Boolean;
var
  Reg: TRegistry;
begin
  Result := False;
  Reg := TRegistry.Create;
  try
    Reg.RootKey := HKEY_CURRENT_USER;
    if Reg.OpenKeyReadOnly('Software\Microsoft\Windows\CurrentVersion\Themes\Personalize') then
    begin
      if Reg.ValueExists('AppsUseLightTheme') then
        Result := Reg.ReadInteger('AppsUseLightTheme') = 0;
    end;
  finally
    Reg.Free;
  end;
end;

procedure TMainForm.WMSettingChange(var Message: TWMSettingChange);
begin
  inherited;
  if (Message.Flag = 0) and (string(Message.Section) = 'ImmersiveColorSet') then
  begin
    // 系统主题发生变化，更新应用程序主题
    UpdateTheme;
  end;
end;

end.

