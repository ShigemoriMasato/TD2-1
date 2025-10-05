# Emitterについて

## 1.概要
そもそもEmitterとはゲームごとに必要な動作が異なるため、とりあえず汎用的なものを作成しました。
 
 これを発展させてParticleを発生させてもいいし、独自に作成しても問題ないです。

 ## 2.使い方
 Initialieして、UpdateとDrawを呼び出すだけです。

 動きを変更したい場合はSetJobsを呼び出して、ParticleJobsをor演算子で組み合わせて渡してください。

 以下は、機能を追加したい場合の説明です。

 ### 2-1.enumに機能を追加
 そのまんまです。
 ### 2-2.処理を追加
 Update関数内に、任意の場所に処理を書いてください。必要は場合は関数を追加してください。クラスも同様
 ### 2-3.保存
 ほかの設定と同じく保存項目に加えたい場合は、次の場所を編集してください。
 - DefaultEmitter::Save() ... 保存したい値をBinaryManagerに登録する。名前を他と被らないようにつける
	- DefaultEmitter::Load() ... Saveで登録した名前が読み込まれていた場合のifを追加して、他と同じように値を挿入する

---
以下は、ParticleResourceについてです。

# ParticleResourceについて
## 1.概要
 そもそもこのエンジンには、モデルやテクスチャなどのリソースと、ID3D12Resourceについてのリソースの二つがあります。
 
 Engine/Resourceが前者、Engine/Render/Resourceが後者です。

 ParticleResourceは、Engine/Render/Resourceに属しています。

 ParticleResourceは、行列とテクスチャをそれぞれ配列でGPUに送信する目的で作成されました。

 ## 2.使い方
 ParticleResourceをInitializeして最大数を登録し、Drawの時にRender::Drawの引数に渡すだけです。

 以下は、値の更新方法の説明です。

 ### 2-1.行列の更新
 position_, rotate_, scale_が登録した最大数だけ存在するので、そこにインスタンスごとの値を入れてください。
 
 行列計算はDrawReadyで行われますが、Render::Draw内で自動で行われるため必要ありません。

 ### 2-n.その他の更新
 先ほどの説明で何となく察したかもですが、DrawReady内ですべての値をGPU送信用にパッケージしています。

 public内にいる変数をいい感じにいじってください。

 ## 3.注意
 もし変更項目を追加したい場合は、Shader/Particle.[][].hlslの構造体も変更してください。

 もっと大きく変更したい場合は、作成元に聞いてください。懇切丁寧に優しく教えます。
