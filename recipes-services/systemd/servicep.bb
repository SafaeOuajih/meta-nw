LICENSE = "CLOSED"
inherit systemd
SYSTEMD_AUTO_ENABLE = "enable"
SYSTEMD_SERVICE_${PN} = "servicep.service"

SRC_URI = "file://service_1.c"
SRC_URI:append = " file://servicep.service "
FILES:${PN} += "\${systemd_unitdir}/system/servicep.service"
FILES:${PN} += "/lib /lib/systemd/"
S = "${WORKDIR}"

do_compile() {
	${CC} service_1.c ${LDFLAGS} -o service_1
}

do_install() {
	install -d ${D}${bindir}
	install -m 0755 service_1 ${D}${bindir}
}

do_install:append() {
  install -d \${D}/\${systemd_unitdir}/system
  install -m 0644 \${WORKDIR}/servicep.service \${D}/\${systemd_unitdir}/system
}
