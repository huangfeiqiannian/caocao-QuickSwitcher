object MainForm: TMainForm
  Left = 0
  Top = 0
  Anchors = [akLeft, akTop, akRight, akBottom]
  Caption = #31383#21475#31649#29702#22120
  ClientHeight = 411
  ClientWidth = 648
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object PageControl1: TPageControl
    Left = 0
    Top = 0
    Width = 648
    Height = 377
    ActivePage = TabSheet3
    Align = alTop
    Anchors = [akLeft, akTop, akRight, akBottom]
    TabOrder = 0
    object TabSheet1: TTabSheet
      Caption = #31383#21475#35268#21017
      DesignSize = (
        640
        349)
      object ListView1: TListView
        AlignWithMargins = True
        Left = 3
        Top = 0
        Width = 551
        Height = 345
        Anchors = [akLeft, akTop, akRight, akBottom]
        Columns = <
          item
            Caption = #31383#21475#21517#31216
            Width = 150
          end
          item
            Caption = #31383#21475#26631#39064
            Width = 150
          end
          item
            Caption = #31383#21475#31867#21517
            Width = 80
          end
          item
            Caption = #31383#21475#26679#24335
            Width = 80
          end>
        ReadOnly = True
        RowSelect = True
        TabOrder = 0
        ViewStyle = vsReport
        OnDblClick = ListView1DblClick
      end
      object Panel1: TPanel
        Left = 555
        Top = 0
        Width = 85
        Height = 349
        Align = alRight
        BevelOuter = bvNone
        TabOrder = 1
        object btnNew: TButton
          Left = 5
          Top = 70
          Width = 75
          Height = 25
          Align = alCustom
          Caption = #26032#24314
          TabOrder = 0
          OnClick = btnNewClick
        end
        object btnEdit: TButton
          Left = 5
          Top = 39
          Width = 75
          Height = 25
          Caption = #32534#36753
          TabOrder = 1
          OnClick = btnEditClick
        end
        object btnDelete: TButton
          Left = 5
          Top = 8
          Width = 75
          Height = 25
          Caption = #21024#38500
          TabOrder = 2
          OnClick = btnDeleteClick
        end
      end
    end
    object TabSheet2: TTabSheet
      Caption = #28909#38190
      object ListView2: TListView
        Left = 0
        Top = 0
        Width = 549
        Height = 349
        Align = alLeft
        Anchors = [akLeft, akTop, akRight, akBottom]
        Columns = <
          item
            Caption = #28909#38190#21517#31216
            Width = 200
          end
          item
            Caption = #28909#38190
            Width = 100
          end>
        ReadOnly = True
        RowSelect = True
        TabOrder = 0
        ViewStyle = vsReport
        OnDblClick = ListView2DblClick
      end
      object Panel2: TPanel
        Left = 555
        Top = 0
        Width = 85
        Height = 349
        Align = alRight
        BevelOuter = bvNone
        TabOrder = 1
        object btnNewRule: TButton
          Left = 5
          Top = 8
          Width = 75
          Height = 25
          Caption = #26032#24314
          TabOrder = 0
          OnClick = btnNewRuleClick
        end
        object btnEditRule: TButton
          Left = 5
          Top = 39
          Width = 75
          Height = 25
          Caption = #32534#36753
          TabOrder = 1
          OnClick = btnEditRuleClick
        end
        object btnDeleteRule: TButton
          Left = 5
          Top = 70
          Width = 75
          Height = 25
          Caption = #21024#38500
          TabOrder = 2
          OnClick = btnDeleteRuleClick
        end
      end
    end
    object TabSheet3: TTabSheet
      Caption = #20854#20182#35774#32622
      object GroupBox1: TGroupBox
        Left = 3
        Top = 3
        Width = 604
        Height = 206
        Caption = #20854#20182#35774#32622
        TabOrder = 0
        object Label1: TLabel
          Left = 16
          Top = 24
          Width = 72
          Height = 13
          Caption = #26368#22823#26631#39064#38271#24230
        end
        object Label2: TLabel
          Left = 16
          Top = 59
          Width = 48
          Height = 13
          Caption = #28909#38190#31867#22411
        end
        object Label3: TLabel
          Left = 16
          Top = 94
          Width = 48
          Height = 13
          Caption = #40664#35748#28909#38190
        end
        object Label4: TLabel
          Left = 16
          Top = 130
          Width = 48
          Height = 13
          Caption = #26263#40657#27169#24335
        end
        object edtMaxTitleLength: TSpinEdit
          Left = 112
          Top = 21
          Width = 65
          Height = 22
          MaxValue = 1000
          MinValue = 10
          TabOrder = 0
          Value = 50
        end
        object cbbHKType: TComboBox
          Left = 112
          Top = 56
          Width = 145
          Height = 21
          Style = csDropDownList
          TabOrder = 1
          Items.Strings = (
            'HK_POLLING'
            'HK_HOOK')
        end
        object edtDefaultKeys: TEdit
          Left = 112
          Top = 91
          Width = 473
          Height = 21
          TabOrder = 2
        end
        object chkRunAsAdmin: TCheckBox
          Left = 16
          Top = 168
          Width = 145
          Height = 17
          Caption = #20197#31649#29702#21592#36523#20221#36816#34892
          TabOrder = 3
          OnClick = chkRunAsAdminClick
        end
        object cbbTreeTheme: TComboBox
          Left = 112
          Top = 127
          Width = 145
          Height = 21
          Style = csDropDownList
          TabOrder = 4
          OnChange = cbbTreeThemeChange
        end
      end
    end
  end
  object btnExit: TButton
    Left = 561
    Top = 383
    Width = 75
    Height = 25
    Caption = #36864#20986
    TabOrder = 1
    OnClick = btnExitClick
  end
  object btnReload: TButton
    Left = 480
    Top = 383
    Width = 75
    Height = 25
    Caption = #24212#29992
    TabOrder = 2
    OnClick = btnReloadClick
  end
end
