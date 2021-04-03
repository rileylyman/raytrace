project('raytracer', 'cpp')

cmake = import('cmake')

sdl_subproj = cmake.subproject('SDL')
sdl_dep = sdl_subproj.dependency('SDL2')

glm_subproj = cmake.subproject('glm')
glm_dep = glm_subproj.dependency('glm')

sources = [
  'Main.cc',
  'Math.cc',
  'Sdl.cc',
  ]

executable('raytrace', sources,
  dependencies : [sdl_dep, glm_dep]
  )
