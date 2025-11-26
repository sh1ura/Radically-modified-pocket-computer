# PCB データ

KiCad でデザインしたPCBのデータを公開します。

![ki2](https://github.com/user-attachments/assets/2601b8d6-2a19-42c4-8434-f2a83632255b)


## 設計手順

1. キーマトリクスの配置を検討（9x10=19本のGPIOで読み取り）
2. 回路図作成
3. キーボードの導電ゴム接点をフットプリントエディタで作成
4. オリジナルの基板をスキャン
5. 画像に合わせてフットプリントを配置

## 作成結果(JLCPCB発注）

![pcb1s](https://github.com/user-attachments/assets/5b7ca455-f0b2-4f75-adbe-2367a2781a58)

![pcb2s](https://github.com/user-attachments/assets/f55f321a-a0c4-4b6b-9459-9a284655765b)

## 搭載する部品

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
* その他
  - L字ピンヘッダ
  - ピンソケットケーブル
  
