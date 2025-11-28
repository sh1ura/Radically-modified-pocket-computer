# ポケコン Linux 化改造

![appearances](https://github.com/user-attachments/assets/4799278b-5afe-4ecb-b594-a3569754d43f)

1988年のポケットコンピュータ（ポケコン） SHARP PC-G801 を改造し、キーボードとディスプレイがついた Linux マシン化しました。

制作過程等は[こちら](https://shiura.com/dfab/sharp-mod/index.html)をごらんください。

### 動画

[![pockecom](https://github.com/user-attachments/assets/01cd19e7-72fa-4902-9953-ad860ec31da8)](https://youtu.be/T_GSy2BSDts)

## 概要

* Raspberry Pi Zero 2 W を内蔵
  - Linux (Raspberry Pi OS) が動作
  - C や Python など様々な言語でプログラミングが可能
  - WiFi でネットワークに接続でき、apt によるソフト追加・更新、pip による Python パッケージ追加など可能
* ESP32-S3 を内蔵
  - キーボード読み取り、ディスプレイ表示を担当
  - ターミナルは VT100 のサブセットで、emacs 等が使用可能
  - ESP32 単体で起動すると、RPN 関数電卓として動作
* 裏面・側面に至るまで、オリジナルの外観をキープ
  - オリジナル同様、単３（ニッケル水素電池）４本で動作
 
<img width="800" src="https://github.com/user-attachments/assets/febd4b89-2eae-4f60-8c1f-658a58cf50a8">

### 回路・部品構成

![design](https://github.com/user-attachments/assets/cc3e37bd-570f-45a6-8252-1ceb3eaf2b97)

 ## 提供物

 * 改造用の PCB （基板）のデータ
 * ESP32 上のコード（VT100サブセットターミナルとRPN関数電卓を統合したもの）
 * ディスプレイを固定するための治具の3Dモデル
