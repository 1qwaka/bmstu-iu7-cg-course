#ifndef PREDEFINEDSCENES_H
#define PREDEFINEDSCENES_H


#include "scene.h"
class PredefinedScenes
{
public:
    PredefinedScenes();

    static void simple(Scene& scene)
    {
        //cam
        scene.cam.d = 1;
        scene.cam.vw = 2;
        scene.cam.vh = 2;
        scene.cam.pos = {0, 0, 0};
        scene.cam.rot = {0, 0, 0};

        //
        scene.add(new Sphere{
                      QVector3D{-2, 0, 4},
                      1,
                      Material{Qt::green, 100}
                  });
        scene.add(new Sphere{
                      QVector3D{2, 0, 4},
                      1,
                      Material{QColor{0, 0, 255}, 1},
                  });
    //    scene.add(new Sphere{
    //                  QVector3D{0, 0, 4},
    //                  1,
    //                  Material{QColor{0, 30, 50}, 125, 0, 1.5},
    //              });
//        scene.add(new Sphere{
//                      QVector3D{0, 0, 9},
//                      1,
//                      Material{QColor{128, 128, 255}, 10},
//                  });
        scene.add(new Sphere{
                      QVector3D{0, -5001, 0},
                      5000,
                      Material{QColor{255, 255, 0}, 1000},
                  });
        scene.add(new Lens{
                      QVector3D{0, 0, 2},
                      QVector3D{0, 0, 0},
                      3,
                      5,
                      5
                  });
    //    scene.add(new PolyObject{
    //                  QList<Tri>{
    //                      {{-1, 2, 3},
    //                      {-1, 1, 3},
    //                      {0, 1, 3}}
    //                  },
    //                  QList<QVector3D>{
    //                      {0, 0, -1}
    //                  },
    //                   Material{QColor{255, 255, 0}, 1},
    //              });

//        scene.add(PolyObject::make_parallel({0.3 , -0.4, 2}, {}, {1, 1, 1}, {}));
//        scene.add(PolyObject::make_pyramid({0, 0, 2}, {}, 0.5, 0.5, {}));
//        scene.add(PolyObject::make_prism({0, 0, 3}, {}, 0.5, 0.5, {}));

        scene.add(new Light{
                      0.6,
                      {0, 1, 1},
                      {},
                      LightType::POINT
                  });
        scene.add(new Light{
                      0.05,
                      {},
                      {},
                      LightType::AMBIENT
                  });
    //    scene.add(new Light{
    //                  0.2,
    //                  {},
    //                  {1, 4, 4},
    //                  LightType::DIRECTIONAL
    //              });

    }

    static void cubes(Scene& scene)
    {
        scene.add(new Light{
                      0.6,
                      {0, 1, 1},
                      {},
                      LightType::POINT
                  });
        scene.add(new Light{
                      0.05,
                      {},
                      {},
                      LightType::AMBIENT
                  });
        scene.add(new Lens{
                      QVector3D{0, 0, 3},
                      QVector3D{0, 0, 0},
                      3,
                      5,
                      5
                  });
        scene.cam.d = 1;
        scene.cam.vw = 2;
        scene.cam.vh = 2;
        scene.cam.pos = {0, 0, 0};
        scene.cam.rot = {0, 0, 0};


        for (int x = -2; x <= 2; x++)
        {
            for (int y = -2; y <= 2; y++)
            {
                scene.add(PolyObject::make_parallel({(float)x, (float)y, 5},
                                                    {0, 0, float(y * 20 + x)},
                                                    {0.5, 0.5, 0.5},
                                                    Material{QColor{
                                                                 std::clamp(128 + (int)(std::abs(x) / 5.0 * 128)    , 0, 255),
                                                                 std::clamp(128 + (int)(std::abs(y) / 5.0 * 128)    , 0, 255),
                                                                 std::clamp(128 + (int)(std::abs(y + x) / 5.0 * 128), 0, 255),
                                                             }
                                                    }));
            }
        }

    }


    static void spheres(Scene& scene)
    {
        scene.add(new Light{
                      0.6,
                      {0, 1, 1},
                      {},
                      LightType::POINT
                  });
        scene.add(new Light{
                      0.05,
                      {},
                      {},
                      LightType::AMBIENT
                  });
        scene.add(new Lens{
                      QVector3D{0, 0, 3},
                      QVector3D{0, 0, 0},
                      3,
                      5,
                      5
                  });
        scene.cam.d = 1;
        scene.cam.vw = 2;
        scene.cam.vh = 2;
        scene.cam.pos = {0, 2, -2};
        scene.cam.rot = {20, 0, 0};

        for (int x = -2; x <= 2; x++)
        {
            for (int y = -2; y <= 2; y++)
            {
                scene.add(new Sphere({(float)x, (float)y, float(x + y)}, 0.5,Material{QColor{
                                                            std::clamp(200 + (int)(std::abs(x) / 5.0 * 128), 0, 255),
                                                            std::clamp(150 + (int)(std::abs(y) / 5.0 * 128), 0, 255),
                                                            std::clamp(200 + (int)(std::abs(y + x) / 5.0 * 128), 0, 255),
                                                        }}
                                     ));
            }
        }

    }



    static void various(Scene& scene)
    {
        scene.add(new Light{
                      0.6,
                      {0, 1, 1},
                      {},
                      LightType::POINT
                  });
        scene.add(new Light{
                      0.05,
                      {},
                      {},
                      LightType::AMBIENT
                  });
        scene.add(new Lens{
                      QVector3D{0, 0, 3},
                      QVector3D{0, 0, 0},
                      3,
                      5,
                      5
                  });
        scene.cam.d = 1;
        scene.cam.vw = 2;
        scene.cam.vh = 2;
        scene.cam.pos = {0, 2, 0};
        scene.cam.rot = {20, 0, 0};

        for (int x = -2; x < 0; x++){
            for (int y = -2; y < 0; y++){
                scene.add(new Sphere({(float)x, (float)y, 4}, 0.5, Material{QColor{
                                                            std::clamp(200 + (int)(std::abs(x) / 5.0 * 128), 0, 255),
                                                            std::clamp(150 + (int)(std::abs(y) / 5.0 * 128), 0, 255),
                                                            std::clamp(200 + (int)(std::abs(y + x) / 5.0 * 128), 0, 255),
                                                        }}
                                     ));
            }
        }

        for (int x = 0; x <= 2; x++){
            for (int y = -2; y < 0; y++){
                scene.add(PolyObject::make_parallel({(float)x, (float)y, 5},
                                                    {x * 30.0f, 0, y * 20.0f + x},
                                                    {0.5, 0.5, 0.5},
                                                    Material{QColor{
                                                                 std::clamp(128 + (int)(std::abs(x) / 3.0 * 128)    , 0, 255),
                                                                 std::clamp(128 + (int)(std::abs(y) / 3.0 * 128)    , 0, 255),
                                                                 std::clamp(150 + (int)(std::abs(y + x) / 3.0 * 128), 0, 255),
                                                             }
                                                    }));
            }
        }

        for (int x = 0; x <= 2; x++){
            for (int y = 0; y <= 2; y++){
                scene.add(PolyObject::make_prism(   {(float)x, (float)y, 6},
                                                    {0, 0, float(y * 20 + x)},
                                                    0.5 + x * 0.1f,
                                                    0.4,
                                                    Material{QColor{
                                                                 std::clamp(128 + (int)(std::abs(x) / 3.0 * 128)    , 0, 255),
                                                                 std::clamp(128 + (int)(std::abs(y) / 3.0 * 128)    , 0, 255),
                                                                 std::clamp(150 + (int)(std::abs(y + x) / 3.0 * 128), 0, 255),
                                                             }
                                                    }));
            }
        }

        for (int x = -2; x < 0; x++){
            for (int y = 0; y <= 2; y++){
                scene.add(PolyObject::make_pyramid({(float)x, (float)y, 7},
                                                    {0, 0, float(y * 20 + x)},
                                                    0.5 + x * 0.1f,
                                                    0.4,
                                                    Material{QColor{
                                                                 std::clamp(128 + (int)(std::abs(x) / 3.0 * 128)    , 0, 255),
                                                                 std::clamp(128 + (int)(std::abs(y) / 3.0 * 128)    , 0, 255),
                                                                 std::clamp(150 + (int)(std::abs(y + x) / 3.0 * 128), 0, 255),
                                                             }
                                                    }));
            }
        }

    }
};

#endif // PREDEFINEDSCENES_H
