import pytest


@pytest.fixture
def run_transition_test(prepare_and_run_transition):
    def _run(ori_idf_text: str, expected_idf_text: str):
        prepare_and_run_transition(
            ori_idf_text=ori_idf_text,
            expected_idf_text=expected_idf_text,
            version_ori=(26, 1, 0),
            version_new=(26, 2, 0),
        )

    return _run
