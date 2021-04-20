export ARCH="arm64"
export CORES=`getconf _NPROCESSORS_ONLN`
export CROSS_COMPILE="aarch64-linux-gnu-"
export WORKDIR="$(pwd)"
export PATH="${WORKDIR}/../gcc-linaro-7.3.1-2018.05-x86_64_aarch64-linux-gnu/bin:$PATH"

if [ x"$1" = x"mrprober" ]; then
   rm -rf build 2>/dev/null || true
   make distclean
fi

if [ ! -d build ]; then
    mkdir -p build
fi

ATFW_OPT_r8a774c0='LSI=G2E RCAR_SA0_SIZE=0 RCAR_AVS_SETTING_ENABLE=0 RZG_EK874=1 RZG_EK874_EMMC=1 PMIC_ROHM_BD9571=0 RCAR_SYSTEM_SUSPEND=0 RCAR_DRAM_DDR3L_MEMCONF=1 RCAR_DRAM_DDR3L_MEMDUAL=1 SPD=none RCAR_LOSSY_ENABLE=0 RZG_DRAM_ECC_FULL=0 RCAR_RPC_HYPERFLASH_LOCKED=0 RZG2_SECURE_BOOT=0 RCAR_SA6_TYPE=1'

make O=build PLAT=rcar ${ATFW_OPT_r8a774c0} bl2 bl31 dummytool

rm -rf deploy/g2e-emmc 2>/dev/null || true
mkdir -p deploy/g2e-emmc

cp -v build/rcar/release/bl2.srec deploy/g2e-emmc/bl2.srec
cp -v build/rcar/release/bl31.srec deploy/g2e-emmc/bl31.srec
cp -v tools/dummy_create/bootparam_sa0.srec deploy/g2e-emmc/bootparam_sa0.srec
cp -v tools/dummy_create/cert_header_sa6.srec deploy/g2e-emmc/cert_header_sa6.srec
