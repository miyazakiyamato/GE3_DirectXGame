import bpy

#オペレータ 頂点を伸ばす
class MYADDON_OT_stretch_vertex(bpy.types.Operator):
    bl_idname = "myaddon.myaddon_ot_stretch_vertex"     #Blenderがクラスを識別する為の固有の文字列
    bl_label = "頂点を伸ばす"                            #メニューのラベルとして表示される文字列
    bl_description = "頂点座標を引っ張って伸ばします"      #説明表示用の文字列
    bl_options = {'REGISTER','UNDO'}                     #REDO,UNDO可能オプション
    #メニューを実行したときに呼ばれるコールバック関数
    def execute(self,context):
        bpy.data.objects["Cube"].data.vertices[0].co.x += 1.0
        print("頂点を伸ばしました。")
        #オペレータの命令終了を通知
        return {'FINISHED'}
