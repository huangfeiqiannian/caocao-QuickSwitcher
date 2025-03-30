program Setting;

{$R *.dres}

uses
  Vcl.Forms,
  Main in 'Main.pas' {MainForm},
  HotKeyEdit in 'HotKeyEdit.pas' {HotKeyEditForm},
  WindowRuleEdit in 'WindowRuleEdit.pas' {WindowRuleEditForm},
  Vcl.Themes,
  Vcl.Styles;

{$R *.res}

begin
  Application.Initialize;
  Application.MainFormOnTaskbar := True;
  TStyleManager.TrySetStyle('Aqua Light Slate');
  Application.CreateForm(TMainForm, MainForm);
  Application.CreateForm(TWindowRuleEditForm, WindowRuleEditForm);
  Application.Run;
end.
