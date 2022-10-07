library IEEE;
use IEEE.STD_LOGIC_1164.all;
use IEEE.std_logic_unsigned.all;

entity BCD is
	port(	CLK: in std_logic;
			D: in std_logic_vector(4 downto 0);
			A: buffer std_logic_vector(3 downto 0);
			B: buffer std_logic_vector(3 downto 0));
end BCD;

architecture Behavioral of BCD is
begin
	process(clk)
	begin
		if clk'event and clk = '1' then
			case D is
				when "00000" => A <= "0000"; B <= "0000";
				when "00001" => A <= "0000"; B <= "0001";
				when "00010" => A <= "0000"; B <= "0010";
				when "00011" => A <= "0000"; B <= "0011";
				when "00100" => A <= "0000"; B <= "0100";
				when "00101" => A <= "0000"; B <= "0101";
				when "00110" => A <= "0000"; B <= "0110";
				when "00111" => A <= "0000"; B <= "0111";
				when "01000" => A <= "0000"; B <= "1000";
				when "01001" => A <= "0000"; B <= "1001";
				when "01010" => A <= "0001"; B <= "0000";
				when "01011" => A <= "0001"; B <= "0001";
				when "01100" => A <= "0001"; B <= "0010";
				when "01101" => A <= "0001"; B <= "0011";
				when "01110" => A <= "0001"; B <= "0100";
				when "01111" => A <= "0001"; B <= "0101";
				when "10000" => A <= "0001"; B <= "0110";
				when "10001" => A <= "0001"; B <= "0111";
				when "10010" => A <= "0001"; B <= "1000";
				when "10011" => A <= "0001"; B <= "1001";
				when "10100" => A <= "0010"; B <= "0000";
				when "10101" => A <= "0010"; B <= "0001";
				when "10110" => A <= "0010"; B <= "0010";
				when "10111" => A <= "0010"; B <= "0011";
				when "11000" => A <= "0010"; B <= "0100";
				when "11001" => A <= "0010"; B <= "0101";
				when "11010" => A <= "0010"; B <= "0110";
				when "11011" => A <= "0010"; B <= "0111";
				when "11100" => A <= "0010"; B <= "1000";
				when "11101" => A <= "0010"; B <= "1001";
				when "11110" => A <= "0011"; B <= "0000";
				when "11111" => A <= "0011"; B <= "0001";
			end case;
		end if;
	end process;
end Behavioral;