import sys
import os
import json
import math
from parameterized import parameterized

import KratosMultiphysics.KratosUnittest as KratosUnittest

sys.path.append(os.path.join('..', 'python_scripts'))
import test_helper


class TestSellmeijersRule(KratosUnittest.TestCase):
    def change_material_parameters(self, file_path, kappa, d70):
        # change the values of the pipe elements
        material_file_name = os.path.join(file_path, 'MaterialParameters.json')
        with open(material_file_name, 'r') as parameter_file:
            parameters = json.load(parameter_file)
            parameters['properties'][0]['Material']['Variables']['PERMEABILITY_XX'] = kappa
            parameters['properties'][0]['Material']['Variables']['PERMEABILITY_YY'] = kappa
            parameters['properties'][2]['Material']['Variables']['PIPE_D_70'] = d70
        with open(material_file_name, 'w') as parameter_file:
            json.dump(parameters, parameter_file, indent=4)

    def change_head_level_polder_side(self, file_path, head_level):
        parameter_file_name = os.path.join(file_path, 'ProjectParameters.json')
        with open(parameter_file_name, 'r') as parameter_file:
            parameters = json.load(parameter_file)
            if "Left" in parameters['processes']['constraints_process_list'][0]['Parameters']['model_part_name']:
                parameters['processes']['constraints_process_list'][0]['Parameters'][
                    'reference_coordinate'] = head_level
            else:
                parameters['processes']['constraints_process_list'][1]['Parameters'][
                    'reference_coordinate'] = head_level
        with open(parameter_file_name, 'w') as parameter_file:
            json.dump(parameters, parameter_file, indent=4)

    def model_kratos_run(self, file_path, head, model=None):
        self.change_head_level_polder_side(file_path, head)
        simulation = test_helper.run_kratos(file_path, model)
        pipe_active = test_helper.get_pipe_active_in_elements(simulation)
        length = test_helper.get_pipe_length(simulation)

        model = simulation.model
        return all(pipe_active), length, model

    def linear_search(self, file_path, search_array):
        counter_head = 0
        pipe_length_total = []
        model = None
        while counter_head < len(search_array):
            # check if pipe elements become active
            pipe_active, pipe_length, model = self.model_kratos_run(file_path, search_array[counter_head], model)
            pipe_length_total.append(pipe_length)
            if pipe_active:
                return search_array[counter_head - 1], pipe_length_total[counter_head - 1]
            counter_head = counter_head + 1
        return None, None

    def drange(self, start, stop, step):
        r = start
        while r < stop:
            yield r
            r += step

    def critical_head_loop(self, file_path, kappa, d70, h_c, search_type='linear'):
        self.change_material_parameters(file_path, kappa, d70)
        heads = list(self.drange(h_c - 1, h_c + 2, 0.1))
        critical_head_found = math.nan
        length = math.nan
        if search_type == 'linear':
            critical_head_found, length = self.linear_search(file_path, heads)
        return critical_head_found, length

    @parameterized.expand(
        [('7.10', 3.00E-04, 1.157E-12, 10.29, 11.3, 7.5, 'test_compare_sellmeijer/HeightAquiferD10L30.gid')])
    def test_sellmeijers_rule_height(self, name, d70, kappa, h_c, h_n, length_n, test_name_gid):
        file_path = test_helper.get_file_path(os.path.join('./', test_name_gid))
        initial_directory = os.getcwd()
        os.chdir(file_path)
        critical_head_found, length = self.critical_head_loop(file_path, kappa, d70, h_n, 'linear')
        self.results = {"value_name": name, "test_result_h": h_c, "kratos_results_h": critical_head_found,
                        "equivalent_software_h": h_n, "kratos_results_l": length, "equivalent_software_l": length_n}
        self.assertAlmostEqual(h_n, critical_head_found, 1,
                               f"Critical head kratos: {critical_head_found}, old geo flow {h_n}")
        os.chdir(initial_directory)

    @parameterized.expand(
        [('7.10', 3.00E-04, 1.157E-12, 10.29, 11.3, 7.5, 'test_compare_sellmeijer/HeightAquiferD10L30line')])
    def test_sellmeijers_rule_height_line(self, name, d70, kappa, h_c, h_n, length_n, test_name_line):
        file_path = test_helper.get_file_path(os.path.join('./', test_name_line))
        initial_directory = os.getcwd()
        os.chdir(file_path)
        critical_head_found, length = self.critical_head_loop(file_path, kappa, d70, h_n, 'linear')
        self.results = {"value_name": name, "test_result_h": h_c, "kratos_results_h": critical_head_found,
                        "equivalent_software_h": h_n, "kratos_results_l": length, "equivalent_software_l": length_n}
        self.assertAlmostEqual(h_n, critical_head_found, 1,
                               f"Critical head kratos: {critical_head_found}, old geo flow {h_n}")
        os.chdir(initial_directory)


if __name__ == '__main__':
    KratosUnittest.main()
