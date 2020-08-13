# Sen

日本語の解説が英語の解説の後にあります。

### What's sen ?

Sen is a simple editor built on Gtk4 library.
The word 'sen' is a Japanese word which is written as '箋' in kanji or 'せん' in hiragana.
The meaning is a memo pad or scrap paper.
But sen is a text editor like notepad rather than scrap paper.

Sen has only limited features now and it should be improved.
At the same time, Gtk4 is being developed at present, so sen also needs to be modified.
This means sen is not stable and it should be improved in the near future.

### Requirements

1. Linux operating system
2. meson, ninja
3. Gtk4 (Gtk version 3.99.0)

### Installation

1. Click 'Clone or Download' button, Click 'Download ZIP' in the small dialog.
2. Extract the zip file.
3. Change the current directory to the directory you've extracted the zip file.
4. Type 'meson _build'.
5. Type 'ninja -C _build'.
6. Type '_build/sen' then you can run sen.

### licence

Copyright (C) 2020  ToshioCP (Sekiya Toshio)

Sen is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

Sen is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the [GNU Lesser General Public License](https://www.gnu.org/licenses/lgpl-2.1.html) for more details.

--------------------------

### Senって何？

SenはGtk4ライブラリの上に構築されたシンプルなエディタです。
'sen'は、日本語の漢字の'箋'で、ひらがなでは'せん'と書きます。
その意味は、目印やメモのための紙のことで、付箋や便箋の箋です。
しかし、senはテキストエディタで、付箋のようなものというよりも、ノートパッドのようなものです。

senは極めて限られた機能しか持ち合わせておらず、更に改良すべき現状です。
同時にGtk4も開発段階であるので、senはその開発とともに書き換えられる必要があります。
これは、senが安定版ではなく、開発版であることを意味しています。

### 動作条件

1. Linuxオペレーティングシステム
2. meson, ninja
3. Gtk4 (Gtk version 3.99.0)

### インストール

1. 'Clone or Download'ボタンをクリックし、現れた小さなダイアログの'Download ZIP'をクリックする
2. ダウンロードしたZipファイルを解凍する
3. 端末を起動し、カレントディレクトリをzipファイルを解凍したディレクトリに移動する
2. meson _build とタイプする
3. ninja -C _build とタイプする
4. _build/sen とタイプするとsenを起動することができる

### ライセンス

Copyright (C) 2020  ToshioCP (関谷 敏雄)

senはフリーソフトウェアです。
senは、フリーソフトウェア財団によって発行されたGNU 劣等一般公衆利用許諾契約書(バージョン2.1 か、希望によってはそれ以降のバージョンのうちどれか)の定める条件の下で再頒布または改変することができます。

senは有用であることを願って頒布されますが、*全くの無保証* です。商業可能性の保証や特定の目的への適合性は、言外に示されたものも含め全く存在しません。
詳しくは[GNU 劣等一般公衆利用許諾契約書](https://www.gnu.org/licenses/lgpl-2.1.html)をご覧ください。
八田真行氏による[GNU 劣等一般公衆利用許諾契約書の日本語訳](https://osdn.net/projects/opensource/wiki/licenses%2FGNU_Library_or_Lesser_General_Public_License)もご覧ください。
