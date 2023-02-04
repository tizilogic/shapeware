let project = new Project('shapeware');

project.addFile('Sources/**');
project.addIncludeDir('Sources');
if (typeof noKrinkPlease === 'undefined') {
    await project.addProject('krink');
    project.addFile('Shaders/**');
    project.setDebugDir('Deployment');
    project.addDefine('KINC_NO_WAYLAND');
}
else project.addExclude('Sources/dummy.c');

project.addFile('ext/sht/sht.c');
project.addFile('ext/sht/murmur3.c');
project.addIncludeDir('ext');
project.flatten();
resolve(project);
