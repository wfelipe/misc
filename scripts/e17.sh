pkgs="	libs/eina
	libs/eet
	libs/ecore
	libs/efreet
	libs/embryo
	libs/edje
	libs/evas
	libs/edbus
	apps/e
	libs/ewl
	proto/python-efl/python-ecore
	proto/python-efl/python-edbus
	proto/python-efl/python-edje
	proto/python-efl/python-evas
	"

[ !-z $1 ] || pkgs=$1

[ -d build ] || mkdir build

cd build
[ -d pkgs ] || mkdir pkgs
for pkg in $pkgs
do
	git clone http://git.debian.org/git/pkg-e/$pkg.git
	app=`basename $pkg`
	pushd $app
	dpkg-buildpackage
	popd
	[ -d "../pkgs/$app" ] || mkdir "../pkgs/$app"
	mv *dsc *changes *deb *tar.gz ../pkgs/$app
done
