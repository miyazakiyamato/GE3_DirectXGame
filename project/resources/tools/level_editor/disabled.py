import bpy

#オペレータ カスタムプロパティ['file_name']追加
class MYADDON_OT_add_disabled(bpy.types.Operator):
    bl_idname = "myaddon.myaddon_ot_add_disabled"
    bl_label = "無効フラグ 追加"
    bl_description = "['disabled']カスタムプロパティを追加します"
    bl_options = {"REGISTER","UNDO"}

    def execute(self, context):
        #['disabled']カスタムプロパティを追加
        context.object["disabled"] = True
        return {"FINISHED"}
#パネル ファイル名
class OBJECT_PT_disabled(bpy.types.Panel):
    """オブジェクトのファイルネームパネル"""
    bl_idname = "OBJECT_PT_disabled"
    bl_label = "disabled"
    bl_space_type = "PROPERTIES"
    bl_region_type = "WINDOW"
    bl_context = "object"

    #サブメニューの描画
    def draw(self,context):
        #パネルに項目を追加
        if "disabled" in context.object:
            #既にプロパティがあれば、プロパティを表示
            self.layout.prop(context.object,'["disabled"]',text=self.bl_label)
        else:
            #プロパティがなければ、プロパティ追加ボタンを表示
            self.layout.operator(MYADDON_OT_add_disabled.bl_idname)