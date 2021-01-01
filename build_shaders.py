import os

dir_path = os.path.dirname(os.path.realpath(__file__))

tree = os.walk(dir_path+'/shaders')
for t in tree:
    for file in t[2]:
        if file.endswith('.vert') or file.endswith('.frag'):
            os.system('glslc ' + 'shaders/'+ file + ' -o ' + 'shaders/'+ file[:len(file)-5] + '.spv')
