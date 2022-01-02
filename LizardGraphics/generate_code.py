import regex
import os

def get_func_name(func):
    return regex.search(r"[\w]+",func)[0]

def get_func_args(func):
    return regex.search(r"(?<=[\w]+\()[\s\w:,<>\(\)\*]*(?=\))",func)[0]

def get_args_convertion(lst,start_counter = 0):
    ret = ''
    counter = start_counter
    for arg in lst:
        ret+='to{0}(args[{1}]){2}'.format(arg,str(counter), ',' if counter+1 - start_counter < len(lst) else '')
        counter+=1
    return ret

def get_lg_object(class_name, obj_id):
    str_enum = class_name[0] + '_' + class_name[1:]
    return '{0}[{1}][{2}]'.format('models' if class_name == 'LModel' else 'primitives',upper(str_enum),obj_id)
    
def insert_to_file(filename,str_to_insert,str_trigger_start,str_trigger_end):
    res = ''
    with open(filename,'r') as f:
        res = ''.join([line for line in f])
    res = regex.sub(r'(?<={0})[\x00-\x7F\s]*(?={1})'.format(str_trigger_start,str_trigger_end),str_to_insert,res)
    with open(filename,'w') as f:
        f.write(res)

def gen_type_checking(cpp_classes,classes_enum, pattern):
    res = '' 
    counter = 0
    for c in classes_enum:
        begin_str = 'if(toint(args[0]) == {0})(({1}*){2};'.format(c, cpp_classes[counter], pattern)
        if counter!= 0: begin_str = 'else ' + begin_str 
        res+=begin_str
        counter+=1
    return res

script_dir = os.path.dirname(os.path.realpath(__file__))
filename_h = script_dir + '\interpretator.h'
filename_cpp = script_dir + '\interpretator.cpp'
metadata_filename = script_dir + '\meta.hxx'
insert_trigger_start = r"// ___GENERATED CODE START___ DON'T MODIFY THIS LINE \(OR YOU'L DIE IN AGONY!!!!\)"
insert_trigger_end =   r"// ___GENERATED CODE END___   DON'T MODIFY THIS LINE \(OR YOU'L DIE IN AGONY!!!!\)"

cpp_classes = (
    'LPlane',
    'LCube',
    'LSphere',
    'LIcosphere',
    'LCone',
    'LTorus',
    'LCylinder',
    )
classes_enum = (
    'L_PLANE',
    'L_CUBE',
    'L_SPHERE',
    'L_ICOSPHERE',
    'L_CONE',
    'L_TORUS',
    'L_CYLINDER',
    )
app_class = 'LApp'
lg_object_classes = ('LWidget')

f = open(metadata_filename,'r')
string = (''.join([line for line in f])).replace(' ','')
string = regex.sub(r"const|glm::|std::|::std::|&",'',string)
 
classes = regex.findall(r"(?<=METAGEN_CLASS\()[\w]+",string)
method_groups = regex.findall(r"(?<=WITH_MEMBERS\()[\x00-\x7F\s]+?(?=\);)",string)

str_to_insert_h = '\nstatic std::unordered_map<std::string, std::function<void(LGraphics::LApp* app,const std::vector<std::string>&)>> appFunctionPointers;\n'
str_to_insert_cpp = '\nstd::unordered_map<std::string, std::function<void(LGraphics::LApp* app, const std::vector<std::string>&)>> LGraphics::Interpretator::appFunctionPointers = \n{'

c = 0
tokens = []
for g in method_groups:
    tokens = list(filter(('').__ne__, g.split('\n')))
    for t in tokens:
        name = get_func_name(t.replace('LApp*,',''))
        args = get_func_args(t.replace('LApp*,','')).split(',')
        if classes[c] == app_class:
            str_to_insert_cpp+=('{"'+ t + '",' +'[&](LGraphics::LApp* app, const std::vector<std::string>& args)'
            '{{app->{0}({1});}}}},\n'.format(name,get_args_convertion(args)))
        elif name.find(classes[c]) != - 1:
            str_to_insert_cpp+=('{"'+ t + '",' +'[&](LGraphics::LApp* app, const std::vector<std::string>& args)'
            '{{new {0}(app,{1});}}}},\n'.format(classes[c],get_args_convertion(args,1)))
        else:
            common_part = 'app->primitives[toint(args[0])][toint(args[1])])->{0}({1})'
            str_to_insert_cpp+=('{"'+ t + '",' +'[&](LGraphics::LApp* app, const std::vector<std::string>& args)'
            '{{if (toint(args[0]) == L_MODEL) app->models[toint(args[1])]->{0}({1});'
            'else {{{2}}}}}}},\n'.format(name,get_args_convertion(args,2),gen_type_checking(cpp_classes, classes_enum, 
            common_part.format(name,get_args_convertion(args,2)))))

        #elif classes[c] in lg_object_classes:
            #pass
        #else:
            #raise Exception("add class to lg_object_classes")
    c+=1

str_to_insert_cpp+="};\n"
f.close()

insert_to_file(filename_h,str_to_insert_h,insert_trigger_start,insert_trigger_end)
insert_to_file(filename_cpp,str_to_insert_cpp,insert_trigger_start,insert_trigger_end)
print('code was generated')




