## 「30日でできる！OS自作入門」ハンズオン用リポジトリー
読了📚
### 環境  
- Windows10  
- Hyper-V  
ビルド生成物をvfdとして、起動する

### Tool類について  
著者が用意してくれている、tool類やフォントファイル、tek.cなどは、  
binフォルダを作成し、その中に配置して使っています
ソース管理には含めていません  

### ビルド  
- build.ps1  
日付を引数にビルドします  
ただ、過去については考慮していないです  
- up.ps1  
Hyper-V上に適当なVM作成して起動します  

### 備考
- 30-3について
Hyper-Vの第１世代ではサウンドサポートされていないので、スキップ
