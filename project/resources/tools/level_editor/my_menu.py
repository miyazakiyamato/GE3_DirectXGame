import bpy
import sys

bl_info = sys.modules[__package__].bl_info
# メニュー項目描画
def draw_menu_manual(self, context):
    self.layout.operator("wm.url_open_preset", text = "Manual", icon = 'HELP')
#オペレータ モジュールのimport
from .spawn import MYADDON_OT_spawn_import_symbol
#オペレータ モジュールの生成
from .spawn import MYADDON_OT_spawn_create_symbol 
#オペレータ シーン出力
from .export_scene import MYADDON_OT_export_scene
#オペレータ 頂点を伸ばす
from .stretch_vertex import MYADDON_OT_stretch_vertex
#オペレータ ICO球生成
from .create_ico_sphere import MYADDON_OT_create_ico_sphere
# トップバーの拡張メニュー
class TOPBAR_MT_my_menu(bpy.types.Menu):
    bl_idname = "TOPBAR_MT_my_menu"                        #Blenderがクラスを識別する為の固有の文字列
    bl_label = "MyMenu"                                    #メニューのラベルとして表示される文字列
    bl_description = "拡張メニュー by " + bl_info["author"] #著者表示用の文字列
    # サブメニューの描画
    def draw(self,context):
        # トップバーの「エディターメニュー」に項目（オペレーター）を追加
        self.layout.operator(MYADDON_OT_export_scene.bl_idname,
                             text = MYADDON_OT_export_scene.bl_label)
        self.layout.operator(MYADDON_OT_spawn_import_symbol.bl_idname,
                             text = MYADDON_OT_spawn_import_symbol.bl_label)
        self.layout.operator(MYADDON_OT_spawn_create_symbol.bl_idname,
                             text = MYADDON_OT_spawn_create_symbol.bl_label)
        self.layout.operator(MYADDON_OT_stretch_vertex.bl_idname,
                             text = MYADDON_OT_stretch_vertex.bl_label)
        self.layout.operator(MYADDON_OT_create_ico_sphere.bl_idname,
                             text = MYADDON_OT_create_ico_sphere.bl_label)
        self.layout.operator("wm.url_open_preset",text = "Manual",icon = 'HELP')
    # 既存のメニューにサブメニューを追加
    def submenu(self,context):
        # ID指定でサブメニューを追加
        self.layout.menu(TOPBAR_MT_my_menu.bl_idname)