# ポケコン Linux 化改造

![appearances](https://github.com/user-attachments/assets/69680d92-128a-4977-b02a-c37e9a54f8d6)

1988年のポケットコンピュータ（ポケコン） SHARP PC-G801 を改造し、キーボードとディスプレイがついた Linux マシン化しました。

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

 ## 提供物

 * 改造用の PCB （基板）のデータ
 * ESP32 上のコード（VT100サブセットターミナルとRPN関数電卓を統合したもの）
 * 改造の　Tips
