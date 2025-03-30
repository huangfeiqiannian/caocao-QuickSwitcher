object HotKeyEditForm: THotKeyEditForm
  Left = 0
  Top = 0
  BorderStyle = bsDialog
  Caption = '热键编辑'
  ClientHeight = 169
  ClientWidth = 321
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = True
  Position = poMainFormCenter
  OnCreate = FormCreate
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 24
    Top = 24
    Width = 24
    Height = 13
    Caption = '热键名称'
  end
  object Label2: TLabel
    Left = 26
    Top = 64
    Width = 36
    Height = 13
    Caption = '热键'
  end
  object Label3: TLabel
    Left = 24
    Top = 103
    Width = 36
    Height = 13
    Caption = '热键类型'
  end
  object lblListType: TLabel
    Left = 160
    Top = 103
    Width = 48
    Height = 13
    Caption = '跟随鼠标'
  end
  object edtName: TEdit
    Left = 80
    Top = 21
    Width = 217
    Height = 21
    TabOrder = 0
  end
  object edtHotKey: TEdit
    Left = 80
    Top = 61
    Width = 217
    Height = 21
    ReadOnly = True
    TabOrder = 1
  end
  object chkFollowMouse: TCheckBox
    Left = 90
    Top = 100
    Width = 17
    Height = 22
    TabOrder = 2
  end
  object cbbListType: TComboBox
    Left = 226
    Top = 100
    Width = 71
    Height = 21
    Style = csDropDownList
    TabOrder = 5
  end
  object btnOK: TButton
    Left = 80
    Top = 128
    Width = 75
    Height = 25
    Caption = '确定'
    Default = True
    TabOrder = 3
    OnClick = btnOKClick
  end
  object btnCancel: TButton
    Left = 176
    Top = 128
    Width = 75
    Height = 25
    Cancel = True
    Caption = '取消'
    ModalResult = 2
    TabOrder = 4
  end
end
