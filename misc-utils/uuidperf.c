#include <getopt.h>
#include <time.h>
#include <uuid.h>

#include "optutils.h"

static const char *fmt_lower =
	"%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x";
struct uuid {
        uint32_t        time_low;
        uint16_t        time_mid;
        uint16_t        time_hi_and_version;
        uint16_t        clock_seq;
        uint8_t node[6];
};
static void uuid_unpack(const uuid_t in, struct uuid *uu)
{
        const uint8_t   *ptr = in;
        uint32_t                tmp;

        tmp = *ptr++;
        tmp = (tmp << 8) | *ptr++;
        tmp = (tmp << 8) | *ptr++;
        tmp = (tmp << 8) | *ptr++;
        uu->time_low = tmp;

        tmp = *ptr++;
        tmp = (tmp << 8) | *ptr++;
        uu->time_mid = tmp;

        tmp = *ptr++;
        tmp = (tmp << 8) | *ptr++;
        uu->time_hi_and_version = tmp;

        tmp = *ptr++;
        tmp = (tmp << 8) | *ptr++;
        uu->clock_seq = tmp;

        memcpy(uu->node, ptr, 6);
}

static void uuid_unparse_x(const uuid_t uu, char *out, const char *fmt)
{
	struct uuid uuid;

	uuid_unpack(uu, &uuid);
	sprintf(out, fmt,
		uuid.time_low, uuid.time_mid, uuid.time_hi_and_version,
		uuid.clock_seq >> 8, uuid.clock_seq & 0xFF,
		uuid.node[0], uuid.node[1], uuid.node[2],
		uuid.node[3], uuid.node[4], uuid.node[5]);
}
static void uuid_unparse_old(const uuid_t uu, char *out)
{
	uuid_unparse_x(uu, out, fmt_lower);
}
static void __attribute__((__noreturn__)) usage(void)
{
	FILE *out = stdout;
	fputs(USAGE_HEADER, out);
	fprintf(out, _(" %s\n"), program_invocation_short_name);
	fputs(USAGE_SEPARATOR, out);
	fputs(_("A binary to test UUID unparse performance.\n"), out);
	printf(USAGE_HELP_OPTIONS(25));
	printf(USAGE_MAN_TAIL("uuidd(8)"));
	exit(EXIT_SUCCESS);
}


int main(int argc, char **argv)
{
	int	c;
	int	count, len = 1000000;
	uuid_t	uuid;
	char	str[UUID_STR_LEN];
	struct	timeval stop, start;
	const char *uu = "0d1913c6-5768-490c-a335-33d295237383";

	while ((c = getopt(argc, argv, "Vh")) != -1) {
		switch (c) {
		case 'V':
			print_version(EXIT_SUCCESS);
		case 'h':
			usage();
		default:
			errtryhelp(EXIT_FAILURE);
		}
	}
	if (uuid_parse(uu, uuid)) {
		return EXIT_FAILURE;
	}

	count = len;
	gettimeofday(&start, NULL);
	while (count-- > 0) {
		uuid_unparse(uuid, str);
	}
	gettimeofday(&stop, NULL);
	printf("took %lu us\n", (stop.tv_sec - start.tv_sec) * 1000000 +
	       stop.tv_usec - start.tv_usec);
	fprintf(stderr, _("Generated UUID: %s\n"), str);
	count = len;
	gettimeofday(&start, NULL);
	while (count-- > 0) {
		uuid_unparse_old(uuid, str);
	}
	gettimeofday(&stop, NULL);
	printf("took %lu us\n", (stop.tv_sec - start.tv_sec) * 1000000 +
	       stop.tv_usec - start.tv_usec);
	fprintf(stderr, _("Generated UUID: %s\n"), str);

	return EXIT_SUCCESS;
}
