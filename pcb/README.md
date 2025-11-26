# PCB データ

KiCad でデザインしたPCBのデータを公開します。

![ki2](https://github.com/user-attachments/assets/2601b8d6-2a19-42c4-8434-f2a83632255b)

## 設計手順

1. キーマトリクスの配置を検討（9x10=19本のGPIOで読み取り）
2. 回路図作成
3. キーボードの導電ゴム接点をフットプリントエディタで作成
4. オリジナルの基板をスキャン
5. 画像に合わせてフットプリントを配置

![ki1](https://github.com/user-attachments/assets/d8137099-9a85-43da-9056-e058721a7131)


## 作成結果(JLCPCB発注）

![pcb1s](https://github.com/user-attachments/assets/5b7ca455-f0b2-4f75-adbe-2367a2781a58)

![pcb2s](https://github.com/user-attachments/assets/f55f321a-a0c4-4b6b-9459-9a284655765b)

## 搭載・使用する部品

![wirings](https://github.com/user-attachments/assets/aa84326e-32b0-4eaa-aca3-168ca877021b)
![heights](https://github.com/user-attachments/assets/e5b81389-7044-4068-a376-afc29bb5e68a)


### この PCB に搭載される部品

* ESP32-S3 搭載ボード
  - ESP32-S3 DevKitC と同じピン配置ですが、より短い形状のものです。[これ](https://www.amazon.co.jp/ESP32-S3-DevKit-N16R8-%E3%83%9C%E3%83%BC%E3%83%89%E9%96%8B%E7%99%BA%E3%83%9C%E3%83%BC%E3%83%89-%E6%A9%9F%E8%83%BD%E3%83%9E%E3%82%A4%E3%82%AF%E3%83%AD%E3%82%B3%E3%83%B3%E3%83%88%E3%83%AD%E3%83%BC%E3%83%A9-%E7%94%A8-ESP32-S3-DevKit/dp/B0CM7VXJP8?th=1)と同じものだと思います（私は AliExpress で購入）。
  - 片側（１列）だけを PCB に接続します。
* ピンソケット
  - 背が低いタイプが必要です。
  - [シングルピンソケット (低メス) 1×20 (20P)](https://akizukidenshi.com/catalog/g/g103138/)を使用しました。
  - ピンヘッダの取り付け方は[こちら](https://74th.hateblo.jp/entry/2022/12/19/124809)を参考にしてください。
* スライドスイッチ（電源スイッチ）
  - 型番 : SK13D07 と思われます。
  - １回路３接点（３ポジション）のものです。うち２ポジションを電源 ON として使用します。
  - 私は[これ](https://www.amazon.co.jp/dp/B0DKBTV5J2)を購入しました。
* ショットキーバリアダイオード
  - 型番 :
  - SR560
  - Raspberry Pi と ESP32 の両方に電源を供給する場合と、ESP32 だけに供給する場合の２通りを実現するために用います。
  - ちょっと大きめですが、以前[これ](https://www.amazon.co.jp/dp/B07ZQM1Q4N)で購入したストックを使用しました。
* L字ピンヘッダ

### プロジェクト全体に必要な部品

* 3.12インチ 256x64画素 OLED ディスプレイ（ドライバ IC : SSD1322）
  - 上のスペックで検索すると見つかる、[こちら](https://nuneno.cocolog-nifty.com/blog/2019/08/post-4b41cf.html)にあるものです。
  - 販売状態では SPI 通信の状態になっていないものが多いので、表面実装の抵抗器を移動させる必要があります。最近、最初から SPI 接続になっているもの（接続端子が２列でなく１列になったもの）も売られているようです。
  - ２列のピンヘッダが縦向きにはんだ付けされていて、ポケコンの筐体に入らないので、Ｌ字ピンヘッダに付け替えてあります。ピンヘッダの抜き方は[こちら](https://dreamerdream.hateblo.jp/entry/2023/08/07/170000)を参考にしてください。
  - 電源は、ESP32 の3.3V 出力（3V3）を接続しています。このカスタム PCB 上に 3V3 を取り出せるパッドがあります。
* Raspberry Pi Zero 2 W
  - 強力両面テープで貼り付けてあるだけです。
  - このカスタム PCB の power2 のパッドから電源供給します。
  - Ｌ字ピンヘッダで電源供給していましたが、配線抵抗による電圧降下があったため、別途太めの電線で電源を引きました。

## 配線

以下では、電源については記載されていません。

* ESP32 には、PCB 上で GND と 5V に配線されています。
* ディスプレイには、SPI のほか、電源端子へ ESP32 の 3V3 出力を配線しています。SPI のピン配置はソースコードで確認してください。
* Raspberry Pi Zero 2 W には、 PCB 上の power2 のパッドから 5V へ配線しています。もちろん GND の配線も必要です。
* UART は、それぞれの TX 出力を、相手側の RX へ接続してください。

![fig2](https://github.com/user-attachments/assets/5f4b2775-e105-4bfe-a333-35ebf184e4db)
