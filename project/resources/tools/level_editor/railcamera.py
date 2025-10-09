import bpy
import gpu
import gpu_extras.batch
import mathutils

class DrawRailCamera:
    handle = None
    points_drawing = []
    
    def generate_points_from_obj():
        control_points = []
        #現在シーンのオブジェクトリストを走査
        for object in bpy.context.scene.objects:
            if object.get("type") == "ControlPointSpawn":
                control_points.append(mathutils.Vector(object.location.copy()))
        if len(control_points) >= 2:
            segment_count = 100
            DrawRailCamera.points_drawing.clear()
            for i in range(segment_count + 1):
                t = 1.0 / segment_count * i
                pos = DrawRailCamera.catmull_rom_position(control_points, t)
                DrawRailCamera.points_drawing.append(pos)

    def catmull_rom_interpolation(p0, p1, p2, p3, t):
        s = 0.5
        t2 = t * t
        t3 = t2 * t
        e3 = p0 * -1.0 + p1 * 3.0 - p2 * 3.0 + p3
        e2 = p0 * 2.0 - p1 * 5.0 + p2 * 4.0 - p3
        e1 = p0 * -1.0 + p2
        e0 = p1 * 2.0
        return (e3 * t3 + e2 * t2 + e1 * t + e0) * s

    def catmull_rom_position(points, t):
        n = len(points)
        if n < 2:
             raise ValueError("制御点は2点以上必要です")
        if n == 2:
            return points[0].lerp(points[1], t)
        division = n - 1
        area_width = 1.0 / division
        index = int(t / area_width)
        if index >= division:
            index = division - 1
        t0 = area_width * index
        t_2 = (t - t0) / area_width
        t_2 = max(0.0, min(1.0, t_2))
        index0 = max(0, index - 1)
        index1 = index
        index2 = min(n - 1, index + 1)
        index3 = min(n - 1, index + 2)
        p0 = points[index0]
        p1 = points[index1]
        p2 = points[index2]
        p3 = points[index3]
        return DrawRailCamera.catmull_rom_interpolation(p0, p1, p2, p3, t_2)

    def draw_callback():
        DrawRailCamera.generate_points_from_obj()

        if not DrawRailCamera.points_drawing:
            return
        
        vertices = {"pos": [v for v in DrawRailCamera.points_drawing]}
        indices = [[i, i + 1] for i in range(len(DrawRailCamera.points_drawing) - 1)]
        shader = gpu.shader.from_builtin("UNIFORM_COLOR")
        batch = gpu_extras.batch.batch_for_shader(shader, "LINES", vertices, indices=indices)
        shader.bind()
        shader.uniform_float("color", [1.0, 0.0, 0.0, 1.0])  # 赤色
        batch.draw(shader)

    def register_draw():
        if DrawRailCamera.handle is None:
            DrawRailCamera.handle = bpy.types.SpaceView3D.draw_handler_add(
                DrawRailCamera.draw_callback, (), 'WINDOW', 'POST_VIEW')

    def unregister_draw():
        if DrawRailCamera.handle is not None:
            bpy.types.SpaceView3D.draw_handler_remove(DrawRailCamera.handle, 'WINDOW')
            DrawRailCamera.handle = None
