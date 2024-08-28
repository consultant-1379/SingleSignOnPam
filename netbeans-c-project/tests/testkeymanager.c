#include <stdio.h>
#include <stdlib.h>
#include <openssl/sha.h>
#include <time.h>

#include "CUnit/Basic.h"
#include "CUnit/Automated.h"
#include "keymanager.h"
#include "tokenmanager.h"
#include "base64.h"
#include "debug.h"
#include "hasher.h"

#define TEST_COUNTER 11289158L
#define TEST_COUNTER_STRING "11289158"
#define TEST_USER "root"

/*
 * Test suite for the key manager
 */

struct tm *test_date() {
    struct tm *time_slot;
    time_t timer = time(NULL);
    time_slot = localtime(&timer);
    time_slot->tm_year = 112;
    time_slot->tm_mon = 11;
    time_slot->tm_mday = 4;
    time_slot->tm_hour = 16;
    time_slot->tm_min = 11;
    return time_slot;
}

int init_suite(void) {
    return 0;
}

int clean_suite(void) {
    return 0;
}

void testgetmasterkey() {
    unsigned char * expected = "53cur1tyh31md477r";
    unsigned char * output = NULL;
    output = get_master_key();
    if (output != NULL) {
        int result = strcmp(output, expected);
        DBG1("output = \'%s\'", output);
        DBG1("expected = \'%s\'", expected);
        free(output);
        CU_ASSERT(result == 0)
    } else {
        CU_FAIL("error")
    }
}

void testreadfirstline_failure() {
    unsigned char * expected = NULL;
    unsigned char * output = NULL;
    output = read_first_line("/FILE/DOES/NOT/EXIST");
    CU_ASSERT(output == expected)
}

void testgetcounter() {
    write_number(COUNTER_FILE, TEST_COUNTER);
    unsigned long output;
    unsigned long expected = TEST_COUNTER;
    output = get_current_counter();
    CU_ASSERT(output == expected)
}

void testbase64encode() {
    unsigned char * input = "123456789";
    char * expected = "MTIzNDU2Nzg5";
    char * output = base64encode(input, strlen(input));

    int result = strcmp(output, expected);
    DBG1("output = \'%s\'", output);
    DBG1("expected = \'%s\'", expected);
    free(output);
    CU_ASSERT(result == 0)
}

void testtimeslots() {
    get_newest_time_slot();
    get_oldest_time_slot();
    CU_ASSERT(0 == 0)
}

void testkeyedHash() {
    unsigned char * input = "MTIzNDU2Nzg5";
    unsigned char * raw;
    char * expected = "Aq1dKxisuA2mJ69HLSrEgnyviduqZ3tSXTIPT8SXwfc=";
    raw = keyedHash_simple(input, input);
    char *output = base64encode(raw, SHA256_DIGEST_LENGTH);

    int result = strcmp(output, expected);
    DBG1("output = \'%s\'", output);
    DBG1("expected = \'%s\'", expected);
    free(output);
    CU_ASSERT(result == 0)
}

void testsha256() {
    unsigned char * input = "123456789";
    unsigned char * raw;
    unsigned char * expected = "15e2b0d3c33891ebb0f1ef609ec419420c20e320ce94c65fbc8c3312448eb225";
    raw = sha256(input);
    unsigned char *output = hexencode(raw, 32);

    int result = strcmp(output, expected);
    DBG1("output = \'%s\'", output);
    DBG1("expected = \'%s\'", expected);
    free(raw);
    CU_ASSERT(result == 0)
}

void testgeneratesessionkey() {
    struct tm *time_slot = test_date();
    unsigned long counter = TEST_COUNTER;
    unsigned char * raw = NULL;
    raw = generate_session_key(time_slot, counter);
    if (raw == NULL) {
        CU_FAIL("Session key generation failed")
        return;
    }
    char * expected = "XPRPA6cKk5tmxoD4jda4EQLwkEROU8/QXRf5KEOLbbA=";
    char *output = base64encode(raw, SHA256_DIGEST_LENGTH);
    DBG1("output = \'%s\'", output);
    DBG1("expected = \'%s\'", expected);
    int result = strcmp(output, expected);
    free(output);

    CU_ASSERT(result == 0)
}

void testincreasecounter() {
    unsigned long oldcounter = get_current_counter();
    update_counter( 10);
    unsigned long newcounter = get_current_counter();
    CU_ASSERT(newcounter == ( 10))
}

void test_extract_counter_nan() {
    char * input = "asdfds-asdf";
    unsigned long output;
    unsigned long expected = 0L;
    output = extract_counter(input);
    DBG1("output = \'%lu\'", output);
    DBG1("expected = \'%lu\'", expected);
    CU_ASSERT(expected == output)
}

void test_extract_counter_toolong() {
//    This is ULONG_MAX + 2. This should equal one.
    char * input = "4294967297-asdf";
    unsigned long output;
    unsigned long expected = 1L;
    output = extract_counter(input);
    DBG1("output = \'%lu\'", output);
    DBG1("expected = \'%lu\'", expected);
    CU_ASSERT(expected == output)
}

void test_extract_counter_empty() {
    char * input = "-kjh";
    unsigned long output;
    unsigned long expected = NULL;
    output = extract_counter(input);
    CU_ASSERT(expected == output)
}

void test_extract_counter_noseperator() {
    char * input = "kljhlkh";
    unsigned long output;
    unsigned long expected = NULL;
    output = extract_counter(input);
    CU_ASSERT(expected == output)
}

void test_extract_counter_success() {
    char * input = "123456789-Aq1dKxisuA2mJ69HLSrEgnyviduqZ3tSXTIPT8SXwfc=";
    unsigned long output;
    unsigned long expected = 123456789L;
    output = extract_counter(input);
    DBG1("output = \'%lu\'", output);
    DBG1("expected = \'%lu\'", expected);
    CU_ASSERT(expected == output)
}

void test_extract_HMAC() {
    char * input = "123456789-Aq1dKxisuA2mJ69HLSrEgnyviduqZ3tSXTIPT8SXwfc=";
    char * output;
    char * expected = "Aq1dKxisuA2mJ69HLSrEgnyviduqZ3tSXTIPT8SXwfc=";
    output = extract_HMAC(input);
    int result = strcmp(output, expected);
    DBG1("output = \'%s\'", output);
    DBG1("expected = \'%s\'", expected);
    free(output);
    CU_ASSERT(result == 0)
}

void test_extract_HMAC_noseperator() {
    char * input = "";
    char * output;
    char * expected = NULL;
    output = extract_HMAC(input);
    CU_ASSERT(output == expected)
    free(output);
}

void test_extract_HMAC_empty() {
    char * input = "-";
    char * output;
    char * expected = "";
    output = extract_HMAC(input);
    int result = strcmp(output, expected);
    DBG1("output = \'%s\'", output);
    DBG1("expected = \'%s\'", expected);
    free(output);
    CU_ASSERT(result == 0)

}

void test_generate_HMAC() {
    struct tm *time_slot = test_date();
    char * userid = TEST_USER;
    unsigned long counter = TEST_COUNTER;
    unsigned char * raw;
//    unsigned char * expected = "TcHG53wGMfrr+68SoWHVbLwD022kUUkW3aF1Eh6sHYU=";
    unsigned char * expected = "55UJtqufwe5WAPLOP8iqAnRDle4LPAiOyaT7SQMiixY=";
    
    raw = generate_mac(time_slot, userid, counter);
    if (raw == NULL) {
        CU_FAIL("MAC generation failed")
        return;
    }
    unsigned char * output = base64encode(raw, 32);
    int result = strcmp(output, expected);
    DBG1("output = \'%s\'", output);
    DBG1("expected = \'%s\'", expected);
    free(output);
    CU_ASSERT(result == 0)
}

void testvalidate_emptymac() {
    char *given_mac_base64 = "";
    int result = validate_mac(given_mac_base64, TEST_USER, TEST_COUNTER);
    CU_ASSERT(result == 1)
}

void testvalidate_shortmac() {
    char *given_mac_base64 = "lasdlfjsa";
    int result = validate_mac(given_mac_base64, TEST_USER, TEST_COUNTER);
    CU_ASSERT(result == 1)
}

void testvalidatemacnew() {
    struct tm *time_slot = get_newest_time_slot();
    unsigned long counter = 1234;
    char * userid = TEST_USER;
    unsigned char * given_mac = NULL;
    given_mac = generate_mac(time_slot, userid, counter);
    if (given_mac == NULL) {
        CU_FAIL("MAC generation failed")
        return;
    }
    char *given_mac_base64 = base64encode(given_mac, SHA256_DIGEST_LENGTH);
    int result = validate_mac(given_mac_base64, userid, counter);
    free(given_mac_base64);
    CU_ASSERT(result == 0)
}

void testvalidatemacold() {
    struct tm *time_slot = get_oldest_time_slot();
    unsigned long counter = TEST_COUNTER;
    char * userid = TEST_USER;
    unsigned char * given_mac = NULL;
    given_mac = generate_mac(time_slot, userid, counter);
    if (given_mac == NULL) {
        CU_FAIL("MAC generation failed")
        return;
    }
    char *given_mac_base64 = base64encode(given_mac, SHA256_DIGEST_LENGTH);
    int result = validate_mac(given_mac_base64, userid, counter);
    free(given_mac_base64);
    CU_ASSERT(result == 0)
}

void testvalidatecounter() {
    unsigned long counter = 5L;
    int result = validate_counter(counter);
    CU_ASSERT(result == 1)
}

int main() {
    CU_pSuite pSuite = NULL;
    set_debug_level(1);
    
    printf("test\n");
    /* Initialize the CUnit test registry */
    if (CUE_SUCCESS != CU_initialize_registry())
        return CU_get_error();

    /* Add a suite to the registry */
    pSuite = CU_add_suite("testkeymanager", init_suite, clean_suite);
    if (NULL == pSuite) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    /* Add the tests to the suite */
    if (
            
            (NULL == CU_add_test(pSuite, "testgetmasterkey\n", testgetmasterkey))
            || (NULL == CU_add_test(pSuite, "testreadfirstline_failure\n", testreadfirstline_failure))
            || (NULL == CU_add_test(pSuite, "testgetcounter\n", testgetcounter))
            || (NULL == CU_add_test(pSuite, "testincreasecounter\n", testincreasecounter))
            || (NULL == CU_add_test(pSuite, "testsha256\n", testsha256))
            || (NULL == CU_add_test(pSuite, "testkeyedHash\n", testkeyedHash))
            || (NULL == CU_add_test(pSuite, "testtimeslots\n", testtimeslots))
            || (NULL == CU_add_test(pSuite, "testvalidatecounter\n", testvalidatecounter))
            || (NULL == CU_add_test(pSuite, "testgeneratesessionkey\n", testgeneratesessionkey))
            || (NULL == CU_add_test(pSuite, "test_generate_HMAC\n", test_generate_HMAC))
            || (NULL == CU_add_test(pSuite, "testbase64encode\n", testbase64encode))
            || (NULL == CU_add_test(pSuite, "test_extract_counter_empty\n", test_extract_counter_empty))
            || (NULL == CU_add_test(pSuite, "test_extract_counter_success\n", test_extract_counter_success))
            || (NULL == CU_add_test(pSuite, "test_extract_counter_nan\n", test_extract_counter_nan))
            || (NULL == CU_add_test(pSuite, "test_extract_counter_toolong\n", test_extract_counter_toolong))
            || (NULL == CU_add_test(pSuite, "test_extract_counter_noseperator\n", test_extract_counter_noseperator))
            || (NULL == CU_add_test(pSuite, "test_extract_HMAC\n", test_extract_HMAC))
            || (NULL == CU_add_test(pSuite, "test_extract_HMAC_noseperator\n", test_extract_HMAC_noseperator))
            || (NULL == CU_add_test(pSuite, "test_extract_HMAC_empty\n", test_extract_HMAC_empty))
            || (NULL == CU_add_test(pSuite, "testvalidate_emptymac\n", testvalidate_emptymac))
            || (NULL == CU_add_test(pSuite, "testvalidate_shortmac\n", testvalidate_shortmac))
            || (NULL == CU_add_test(pSuite, "testvalidatemacold\n", testvalidatemacold))
            || (NULL == CU_add_test(pSuite, "testvalidatemacnew\n", testvalidatemacnew))
            ) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    /* Run all tests using the CUnit Basic interface */
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_automated_run_tests();
    CU_list_tests_to_file();
    CU_cleanup_registry();
    return CU_get_error();
}
