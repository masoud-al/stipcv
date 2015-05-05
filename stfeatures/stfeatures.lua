

package = newpackage()
package.name = "stfeatures"
package.kind = "exe"
package.language = "c++"


package.bindir="."
--package.config["Debug"].bindir   = "stfeatures_d"
--package.config["Release"].bindir = "stfeatures"

--package.config["Debug"].bindir   = "debug"
--package.config["Release"].bindir = "release"

package.objdir = "."
package.config["Debug"].objdir   = "debug"
package.config["Release"].objdir = "release"




package.config["Debug"].defines         = { "LINUX=1", "DEBUG=1", "_DEBUG=1" };
package.config["Debug"].buildoptions    = { "-D_DEBUG -ggdb -Wall" }

package.config["Release"].defines       = { "LINUX=1", "NDEBUG=1" };
package.config["Release"].buildoptions  = { "-Wall" }

--package.buildoptions = {"'pkg-config --libs opencv'"}

package.includepaths = { 
    "/homes/malipour/include/opencv"
}

package.libpaths = { 
    "/homes/malipour/lib/"
}
 
--package.linkoptions = { "'pkg-config --libs opencv'" }

package.linkoptions = { "-L/homes/malipour/lib -lcxcore -lcv -lhighgui -lcvaux -lml"  }
 
--package.links={ "-lcxcore -lcv -lhighgui -lcvaux -lml" }

--package.linkoptions = { "static-runtime" }


package.files = { matchfiles (
    "*.h",
    "*.cpp"
    )
}
