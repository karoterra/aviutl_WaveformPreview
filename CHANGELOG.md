# Change Log

## 0.3.1 - 2022-12-11
- プロジェクトファイルを開いたときにAviUtlが落ちることがある問題を修正([07f0065](https://github.com/karoterra/aviutl_WaveformPreview/commit/663cb4d5098a1eb9684a1338370703d6e6a1aff6))

## 0.3.0 - 2021-09-21
- プレビュー再生時に再生位置を示すカーソルを表示
  - これに伴いプラグインのメニューを`表示`から`設定`に変更
- IMEを無効化
  - IMEがONになっているためにキー操作ができない、ということを防ぐため
- キー操作の処理をこのプラグインからAviUtl本体に変更
- チャンネルの境界線を表示
- 設定画面を修正
- 動画出力中の一部操作を禁止
- 動画出力後に拡大率が消えたままになるのを修正

## 0.2.0 - 2021-08-26
- キャッシュの管理を別プロセス(`WaveformPreview.exe`)に切り分けることで、
  AviUtl メインプロセスのメモリ圧迫を回避
- 自動スクロール時に正しい波形が描画されるよう修正
- エラー発生時のメッセージボックスを修正
- 現在フレームの色を修正

## 0.1.0 - 2021-08-16
初版公開
