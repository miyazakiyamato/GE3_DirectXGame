import bpy
#オペレータ ICO球生成
class MYADDON_OT_create_ico_sphere(bpy.types.Operator):
    bl_idname = "myaddon.myaddon_ot_create_object"     #Blenderがクラスを識別する為の固有の文字列
    bl_label = "ICO球生成"                              #メニューのラベルとして表示される文字列
    bl_description = "ICO球を生成します"                 #説明表示用の文字列
    bl_options = {'REGISTER','UNDO'}                    #REDO,UNDO可能オプション
    #メニューを実行したときに呼ばれるコールバック関数
    def execute(self,context):
        bpy.ops.mesh.primitive_ico_sphere_add()
        print("ICO球を生成しました。")
        #オペレータの命令終了を通知
        return {'FINISHED'}