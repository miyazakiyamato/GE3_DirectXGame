import bpy
# ブレンダーの登録するアドオン情報
bl_info = {
    "name": "レベルエディタ",
    "author": "Taro Kamata",
    "version": (1,0),
    "blender": (3,3,1),
    "location": "",
    "description": "レベルエディタ",
    "warning": "",
    "wiki_url": "",
    "tracker_url": "",
    "category": "Object"
}
#オペレータ シーン出力
from .export_scene import MYADDON_OT_export_scene
#オペレータ カスタムプロパティ['file_name']追加
from .add_filename import MYADDON_OT_add_filename
#オペレータ カスタムプロパティ['collider']追加
from .add_collider import MYADDON_OT_add_collider
#オペレータ カスタムプロパティ['disabled']追加
from .disabled import MYADDON_OT_add_disabled
#パネル ファイル名
from .file_name import OBJECT_PT_file_name
#パネル コライダー
from .collider import OBJECT_PT_collider
#パネル 無効フラグ
from .disabled import OBJECT_PT_disabled
#オペレータ 頂点を伸ばす
from .stretch_vertex import MYADDON_OT_stretch_vertex
#オペレータ ICO球生成
from .create_ico_sphere import MYADDON_OT_create_ico_sphere
#コライダー描画
from .draw_collider import DrawCollider
# トップバーの拡張メニュー
from .my_menu import TOPBAR_MT_my_menu
#Blenderに登録するクラスリスト
classes = (
    MYADDON_OT_export_scene,
    MYADDON_OT_stretch_vertex,
    MYADDON_OT_create_ico_sphere,
    TOPBAR_MT_my_menu,
    MYADDON_OT_add_filename,
    MYADDON_OT_add_collider,
    MYADDON_OT_add_disabled,
    OBJECT_PT_file_name,
    OBJECT_PT_collider,
    OBJECT_PT_disabled,
)
# Add-On有効化時コールバック
def register():
    #Blenderにクラスを登録
    for cls in classes:
        bpy.utils.register_class(cls)
    # メニューに項目を追加
    bpy.types.TOPBAR_MT_editor_menus.append(TOPBAR_MT_my_menu.submenu)
    #3Dビューに描画関数を追加
    DrawCollider.handle = bpy.types.SpaceView3D.draw_handler_add(DrawCollider.draw_collider,(),"WINDOW","POST_VIEW")
    print("レベルエディタが有効化されました")
    print("bl_info:", bl_info)
# Add-On無効化時コールバック
def unregister():
    # メニューから項目を削除
    bpy.types.TOPBAR_MT_editor_menus.remove(TOPBAR_MT_my_menu.submenu)
    #3Dビューから描画関数を削除
    bpy.types.SpaceView3D.draw_handler_remove(DrawCollider.handle,"WINDOW")
    #Blenderからクラスを削除
    for cls in classes:
        bpy.utils.unregister_class(cls)
    print("レベルエディタが無効化されました")

if __name__ == "__main__":
    register()