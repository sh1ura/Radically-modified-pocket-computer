# ポケコン Linux 化改造

![appearances](https://github.com/user-attachments/assets/f205425d-3159-477d-a48a-aa2f1cca759d)

1988年のポケットコンピュータ（ポケコン） SHARP PC-G801 を改造し、キーボードとディスプレイがついた Linux マシン化しました。

制作過程等は[こちら](https://shiura.com/dfab/sharp-mod/index.html)をごらんください。

## 概要

* Raspberry Pi Zero 2 W を内蔵
  - Linux (Raspberry Pi OS) が動作
  - C や Python など様々な言語でプログラミングが可能
  - WiFi でネットワークに接続でき、apt によるソフト追加・更新、pip による Python パッケージ追加など可能
* ESP32-S3 を内蔵
  - キーボード読み取り、ディスプレイ表示を担当
  - ターミナルは VT100 のサブセットで、emacs 等が使用可能
  - 単体で起動すると、RPN 関数電卓として動作

### 動画

[![pockecom](https://github.com/user-attachments/assets/01cd19e7-72fa-4902-9953-ad860ec31da8)](https://youtu.be/T_GSy2BSDts)

### 回路・部品構成

![design](https://github.com/user-attachments/assets/cc3e37bd-570f-45a6-8252-1ceb3eaf2b97)


 ## 提供物

 * 改造用の PCB （基板）のデータ
 * ESP32 上のコード（VT100サブセットターミナルとRPN関数電卓を統合したもの）
 * ディスプレイを固定するための治具の3Dモデル
